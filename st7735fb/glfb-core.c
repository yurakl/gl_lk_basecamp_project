#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/fb.h>	
#include <linux/mutex.h>
#include <linux/spi/spi.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <video/mipi_display.h>

#include "glfb.h"
extern struct glfb_lcd_par glfb_par; 

static int glfb_init_display(void);
static void glfb_reset(void);
static int glfb_write(u8 data);
static void glfb_write_data(u8 data);
static int glfb_write_data_buf(u8 *txbuf, int size);
static void glfb_write_cmd(u8 data);
static void glfb_run_cfg_script(void);
static void glfb_set_addr_win(int xs, int ys, int xe, int ye);
 
int glfb_print(u16 *buffer, size_t size)
{
	pr_info("st7735fb_print\n");
	glfb_set_addr_win(0, 0, glfb_par.width-1, glfb_par.height-1);
        glfb_write_cmd(MIPI_DCS_WRITE_MEMORY_START);
	glfb_write_data_buf((u8 __force __iomem *) buffer, size);
	glfb_write_cmd(MIPI_DCS_NOP);
	return 0;
}
EXPORT_SYMBOL(glfb_print);



static int glfb_init_display(void)
{
	/* TODO: Need some error checking on gpios */

        /* Request GPIOs and initialize to default values */
        gpio_request_one(glfb_par.rst, GPIOF_OUT_INIT_HIGH, "ST7735 Reset Pin");
        gpio_request_one(glfb_par.dc, GPIOF_OUT_INIT_LOW, "ST7735 Data/Command Pin");

	glfb_reset();

	glfb_run_cfg_script();

	return 0;
}
	
static void glfb_run_cfg_script(void)
{
	int i = 0;
	int max_num = 512;
	pr_info("glfb_run_cfg_script\n");
	while (glfb_par.init_sequence[i] != -3 && i < max_num)
	{ 
		if (glfb_par.init_sequence[i] == -1) {
			glfb_write_cmd(glfb_par.init_sequence[++i]);
		}
		else if (glfb_par.init_sequence[i] == -2) { 
			mdelay(glfb_par.init_sequence[++i]);
		}
		else {
			glfb_write_data(glfb_par.init_sequence[i]);
		}
		i++;
			
	} 
}

static void glfb_set_addr_win(int xs, int ys, int xe, int ye)
{
	glfb_write_cmd(MIPI_DCS_SET_COLUMN_ADDRESS);
	glfb_write_data(0x00);
	glfb_write_data(xs); //st7735_write_data(xs+2);
	glfb_write_data(0x00);
	glfb_write_data(xe);
	glfb_write_cmd(MIPI_DCS_SET_PAGE_ADDRESS);
	glfb_write_data(0x00);
	glfb_write_data(ys); // st7735_write_data(ys+1);
	glfb_write_data(0x00);
	glfb_write_data(ye);
}

static void glfb_reset(void)
{
	/* Reset controller */
	gpio_set_value(glfb_par.rst, 0);
	udelay(10);
	gpio_set_value(glfb_par.rst, 1);
	mdelay(120);
}

static int glfb_write(u8 data)
{
	u8 txbuf[2]; /* allocation from stack must go */

	txbuf[0] = data;

	return spi_write(glfb_par.spi, &txbuf[0], 1);
}

static void glfb_write_data(u8 data)
{
	int ret = 0;

	/* Set data mode */
	gpio_set_value(glfb_par.dc, 1);

	ret = glfb_write(data);
	if (ret < 0)
		pr_err("%s: write data %02x failed with status %d\n", __func__, data, ret);
}

static int glfb_write_data_buf(u8 *txbuf, int size)
{
	/* Set data mode */
	gpio_set_value(glfb_par.dc, 1); 
	/* Write entire buffer */
	return spi_write(glfb_par.spi, txbuf, size);
}

static void glfb_write_cmd(u8 data)
{
	int ret = 0;

	/* Set command mode */
	gpio_set_value(glfb_par.dc, 0);

	ret = glfb_write(data);
	if (ret < 0)
		pr_err("%s: write command %02x failed with status %d\n", __func__, data, ret);
}
 
int glfb_probe(struct spi_device *spi)
{ ;
	int ret = 0, i = 0;
	int vmem_size = glfb_par.width * glfb_par.height * 2;
	 
	gpio_free(glfb_par.cs);
	ret = gpio_request_one(glfb_par.cs, GPIOF_ACTIVE_LOW , "CS_GPIO");
	if (ret < 0) {
		pr_err("%s: %s\n","glfb", "gpio_request_one");
		return ret;
	} 
		 
	spi->mode = SPI_MODE_3;
	spi->max_speed_hz = glfb_par.speed;
	spi->bits_per_word = 8;
	spi->cs_gpio = glfb_par.cs; 
	
	ret = spi_setup(spi);
	
	if (ret < 0) {
		pr_err("%s: %s\n", "glfb", "spi_setup");
		return ret;
	} 
	pr_info("SPI init success\n");	 
	
	//glfb_par = kmalloc(sizeof(struct glfb_par), GFP_KERNEL);

	glfb_par.spi = spi;
	/*
	par->rst = RST_GPIO;
	par->dc = DC_GPIO;
	*/ 
	glfb_par.ssbuf = vzalloc(vmem_size); 
	if (!glfb_par.ssbuf)
		return ret; 
	glfb_init_display();  
	while (i < glfb_par.width * glfb_par.height)
	{
		glfb_par.ssbuf[i] = 0x1f;
		i++;
	} 
	glfb_print(glfb_par.ssbuf, vmem_size);
	mdelay(1000);
        
        i = 0;
	while (i < glfb_par.width * glfb_par.height)
	{
		glfb_par.ssbuf[i] = (u16) 0x0f << 5;
		i++;
	} 
	glfb_print(glfb_par.ssbuf, vmem_size);
        mdelay(1000);
        
        i = 0;
	while (i < glfb_par.width * glfb_par.height)
	{
		glfb_par.ssbuf[i] =(u16) 0x1f << 12;
		i++;
	} 
	glfb_print(glfb_par.ssbuf, vmem_size);
        mdelay(1000); 
        
	pr_info("st7735fb_par init success\n");
	
	return 0;
}

int glfb_remove(struct spi_device *spi)
{
//	struct fb_info *info = spi_get_drvdata(spi);

	spi_set_drvdata(spi, NULL);
	glfb_reset();
/*
	if (info) {
		unregister_framebuffer(info);
		vfree(info->screen_base);	
		framebuffer_release(info);
	}
*/
	/* TODO: release gpios */

	return 0;
}
