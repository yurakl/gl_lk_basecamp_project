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


#define DRVNAME		"ili9341"
#define COMPATIBLE	"glfb"
#define WIDTH		240
#define HEIGHT		320
#define BPP		16
#define CS_GPIO		8
#define RST_GPIO 	27
#define DC_GPIO 	22
#define SPI_BUS_SPEED   12000000

#include "glfb.h"

static const s16 default_init_sequence[] = {
	-1, MIPI_DCS_SOFT_RESET,
	-2, 150,                               /* delay */

	-1, MIPI_DCS_SET_DISPLAY_OFF,
	-2, 500,                               /* delay */

	-1, 0xCF, 0x00, 0xc1, 0x30,
	-1, 0xED, 0x64, 0x03, 0x12, 0x81,
	-1, 0xE8, 0x85, 0x00, 0x78,
	-1, 0xCB, 0x39, 0X2C, 0x00, 0x34, 0x02,
	-1, 0xF7, 0x20,
	-1, 0xEA, 0x00, 0x00,

	/* ------------power control-------------------------------- */
	-1, 0xC0, 0x26,
	-1, 0xC1, 0x11,
	-1, 0xC5, 0x35, 0x3E,
	-1, 0xC5, 0x35, 0x3E,
	-1, 0xC7, 0xBE,
	-1, MIPI_DCS_SET_PIXEL_FORMAT, 0x55,
	-1, 0xB1, 0x00, 0x1B,
	-1, MIPI_DCS_SET_GAMMA_CURVE, 0x01,
	/* ------------display-------------------------------------- */
	-1, 0xB7, 0x07,
	-1, 0xB6, 0x0A, 0x82, 0x27, 0x00,
	-1, MIPI_DCS_EXIT_SLEEP_MODE,
	-2, 100,
	-1, MIPI_DCS_SET_DISPLAY_ON
	-2, 100,	
	
	/* end marker */
	-3
};

struct glfb_lcd_par glfb_par;
EXPORT_SYMBOL(glfb_par);


GLFB_INIT(DRVNAME, COMPATIBLE);

MODULE_ALIAS("spi:" DRVNAME);
MODULE_ALIAS("platform:" DRVNAME);
MODULE_ALIAS("spi:ili9341");
MODULE_ALIAS("platform:ili9341"); 
