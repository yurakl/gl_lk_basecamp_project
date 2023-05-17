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


#define DRVNAME		"st7735fb"
#define COMPATIBLE	"st7735, glfb"
#define WIDTH		128
#define HEIGHT		160
#define BPP		16
#define CS_GPIO		24
#define RST_GPIO 	21
#define DC_GPIO 	20
#define SPI_BUS_SPEED   12000000

#include "glfb.h"
static const s16 default_init_sequence[] = {
	-1, MIPI_DCS_SOFT_RESET,
	-2, 150,                               /* delay */

	-1, MIPI_DCS_EXIT_SLEEP_MODE,
	-2, 500,                               /* delay */

	/* FRMCTR1 - frame rate control: normal mode
	 * frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
	 */
	-1, 0xB1, 0x01, 0x2C, 0x2D,

	/* FRMCTR2 - frame rate control: idle mode
	 * frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
	 */
	-1, 0xB2, 0x01, 0x2C, 0x2D,

	/* FRMCTR3 - frame rate control - partial mode
	 * dot inversion mode, line inversion mode
	 */
	-1, 0xB3, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,

	/* INVCTR - display inversion control
	 * no inversion
	 */
	-1, 0xB4, 0x07,

	/* PWCTR1 - Power Control
	 * -4.6V, AUTO mode
	 */
	-1, 0xC0, 0xA2, 0x02, 0x84,

	/* PWCTR2 - Power Control
	 * VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
	 */
	-1, 0xC1, 0xC5,

	/* PWCTR3 - Power Control
	 * Opamp current small, Boost frequency
	 */
	-1, 0xC2, 0x0A, 0x00,

	/* PWCTR4 - Power Control
	 * BCLK/2, Opamp current small & Medium low
	 */
	-1, 0xC3, 0x8A, 0x2A,

	/* PWCTR5 - Power Control */
	-1, 0xC4, 0x8A, 0xEE,

	/* VMCTR1 - Power Control */
	-1, 0xC5, 0x0E,

	-1, MIPI_DCS_EXIT_INVERT_MODE,

	-1, MIPI_DCS_SET_PIXEL_FORMAT, MIPI_DCS_PIXEL_FMT_16BIT,

	-1, MIPI_DCS_SET_DISPLAY_ON,
	-2, 100,                               /* delay */

	-1, MIPI_DCS_ENTER_NORMAL_MODE,
	-2, 10,                               /* delay */

	/* end marker */
	-3
};

struct glfb_lcd_par glfb_par;
EXPORT_SYMBOL(glfb_par);


GLFB_INIT(DRVNAME, COMPATIBLE);
 
MODULE_ALIAS("spi:" DRVNAME);
MODULE_ALIAS("platform:" DRVNAME);
MODULE_ALIAS("spi:st7735");
MODULE_ALIAS("platform:st7735");						
MODULE_INFO(intree, "Y");

 

