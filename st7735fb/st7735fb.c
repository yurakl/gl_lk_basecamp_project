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
#include "st7735fb.h"
 
static struct st7735fb_par *par;

struct st7735_size st7735_s = {
	.height = HEIGHT,
	.width = WIDTH,
	.bpp =	BPP,
};
//EXPORT_SYMBOL(st7735_s);
	
int st7735fb_print(u16 *buffer, size_t size)
{
	pr_info("st7735fb_print\n");
	st7735_set_addr_win(0, 0, WIDTH-1, HEIGHT-1);
        st7735_write_cmd(ST7735_RAMWR);
	st7735_write_data_buf((u8 __force __iomem *) buffer, size);
	st7735_write_cmd(ST7735_NOP);
	return 0;
}
//EXPORT_SYMBOL(st7735fb_print);


static int st7735fb_init_display(void)
{
	/* TODO: Need some error checking on gpios */

        /* Request GPIOs and initialize to default values */
        gpio_request_one(par->rst, GPIOF_OUT_INIT_HIGH, "ST7735 Reset Pin");
        gpio_request_one(par->dc, GPIOF_OUT_INIT_LOW, "ST7735 Data/Command Pin");

	st7735_reset();

	st7735_run_cfg_script();

	return 0;
}
	
static void st7735_run_cfg_script(void)
{
	int i = 0;
	int end_script = 0;

	do {
		switch (st7735_cfg_script[i].cmd)
		{
		case ST7735_START:
			break;
		case ST7735_CMD:
			st7735_write_cmd(st7735_cfg_script[i].data & 0xff);
			break;
		case ST7735_DATA:
			st7735_write_data(st7735_cfg_script[i].data & 0xff);
			break;
		case ST7735_DELAY:
			mdelay(st7735_cfg_script[i].data);
			break;
		case ST7735_END:
			end_script = 1;
		}
		i++;
	} while (!end_script);
}

static void st7735_set_addr_win(int xs, int ys, int xe, int ye)
{
	st7735_write_cmd(ST7735_CASET);
	st7735_write_data(0x00);
	st7735_write_data(xs); //st7735_write_data(xs+2);
	st7735_write_data(0x00);
	st7735_write_data(xe);
	st7735_write_cmd(ST7735_RASET);
	st7735_write_data(0x00);
	st7735_write_data(ys); // st7735_write_data(ys+1);
	st7735_write_data(0x00);
	st7735_write_data(ye);
}

static void st7735_reset(void)
{
	/* Reset controller */
	gpio_set_value(par->rst, 0);
	udelay(10);
	gpio_set_value(par->rst, 1);
	mdelay(120);
}

static int st7735_write(u8 data)
{
	u8 txbuf[2]; /* allocation from stack must go */

	txbuf[0] = data;

	return spi_write(par->spi, &txbuf[0], 1);
}

static void st7735_write_data( u8 data)
{
	int ret = 0;

	/* Set data mode */
	gpio_set_value(par->dc, 1);

	ret = st7735_write(data);
	if (ret < 0)
		pr_err("%s: write data %02x failed with status %d\n", __func__, data, ret);
}

static int st7735_write_data_buf(u8 *txbuf, int size)
{
	/* Set data mode */
	gpio_set_value(par->dc, 1); 
	/* Write entire buffer */
	return spi_write(par->spi, txbuf, size);
}

static void st7735_write_cmd(u8 data)
{
	int ret = 0;

	/* Set command mode */
	gpio_set_value(par->dc, 0);

	ret = st7735_write(data);
	if (ret < 0)
		pr_err("%s: write command %02x failed with status %d\n", __func__, data, ret);
}
 
static int st7735fb_probe (struct spi_device *spi)
{
	int ret = 0, i = 0;;
	int vmem_size = WIDTH * HEIGHT * 2;
	
	pr_info("SPI init\n");
	gpio_free(CS_GPIO);
	ret = gpio_request_one(CS_GPIO, GPIOF_ACTIVE_LOW , "CS_GPIO");
	if (ret < 0) {
		pr_err("%s: %s\n", DRVNAME, "gpio_request_one");
		return ret;
	} 
		 
	spi->mode = SPI_MODE_3;
	spi->max_speed_hz = SPI_BUS_SPEED;
	spi->bits_per_word = 8;
	spi->cs_gpio = CS_GPIO; 
	
	ret = spi_setup(spi);
	
	if (ret < 0) {
		pr_err("%s: %s\n", DRVNAME, "spi_setup");
		return ret;
	} 
	pr_info("SPI init success\n");
	
	pr_info("st7735fb_par init\n");
	 
	
	par = kmalloc(sizeof(struct st7735fb_par), GFP_KERNEL);

	par->spi = spi;
	par->rst = RST_GPIO;
	par->dc = DC_GPIO;
	
	par->ssbuf = vzalloc(vmem_size);
	if (!par->ssbuf)
		return ret;
		 
	st7735fb_init_display(); 
	
	while (i < WIDTH * HEIGHT)
	{
		par->ssbuf[i] = 0x1f;
		i++;
	} 
	st7735fb_print(par->ssbuf, vmem_size);
	mdelay(1000);
        
        i = 0;
	while (i < WIDTH * HEIGHT)
	{
		par->ssbuf[i] = 0x0f << 6;
		i++;
	} 
	st7735fb_print(par->ssbuf, vmem_size);
        mdelay(1000);
        
        i = 0;
	while (i < WIDTH * HEIGHT)
	{
		par->ssbuf[i] = 0x1f << 12;
		i++;
	} 
	st7735fb_print(par->ssbuf, vmem_size);
        mdelay(1000); 
        
	pr_info("st7735fb_par init success\n");
	
	return 0;
}

static int st7735fb_remove(struct spi_device *spi)
{
//	struct fb_info *info = spi_get_drvdata(spi);

	spi_set_drvdata(spi, NULL);
	st7735_reset();
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

static const struct of_device_id st7735fb_of_device_id[] = {
	{.compatible = "st7735fb"},
	{},
};

MODULE_DEVICE_TABLE(of, st7735fb_of_device_id);

struct spi_driver st7735fb_driver = {
	.probe = st7735fb_probe,
	.remove = st7735fb_remove,
	.driver = {
		.name = DRVNAME,
		.owner = THIS_MODULE,
		.of_match_table = st7735fb_of_device_id,
	},
};
 

static int __init st7735fb_init(void)
{
	return spi_register_driver(&st7735fb_driver);
}

static void __exit st7735fb_exit(void)
{
	symbol_put_addr((void **)&st7735fb_print);
	spi_unregister_driver(&st7735fb_driver);
}
 

module_init(st7735fb_init);
module_exit(st7735fb_exit);

MODULE_DESCRIPTION("FB driver for ST7735 display controller");
MODULE_AUTHOR("Yurii Klysko");
MODULE_LICENSE("GPL");
