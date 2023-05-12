#define DRVNAME		"st7735fb"
#define WIDTH		128
#define HEIGHT		160
#define BPP		16


#define CS_GPIO		16
#define RST_GPIO 	21
#define DC_GPIO 	20
#define SPI_BUS_SPEED   12000000
/* Supported display modules */
#define ITDB02	0 

struct st7735_size {
	u16 height;
	u16 width;
	u16 bpp;
};
/* Init script function */
struct st7735_function {
	u16 cmd;
	u16 data;
};

/* Init script commands */
enum st7735_cmd {
	ST7735_START,
	ST7735_END,
	ST7735_CMD,
	ST7735_DATA,
	ST7735_DELAY
};

struct st7735fb_par {
	struct spi_device *spi; 
	u16 *ssbuf;
	int rst;
	int dc;
};
 
 
static int st7735fb_init_display(void);
static void st7735_reset(void);
static int st7735_write(u8 data);
static void st7735_write_data(u8 data);
static int st7735_write_data_buf(u8 *txbuf, int size);
static void st7735_write_cmd(u8 data);
static void st7735_run_cfg_script(void);
static void st7735_set_addr_win(int xs, int ys, int xe, int ye);


int st7735fb_print(u16 *buffer, size_t size); 

/* ST7735 Commands */
#define ST7735_NOP	0x0
#define ST7735_SWRESET	0x01
#define ST7735_RDDID	0x04
#define ST7735_RDDST	0x09
#define ST7735_SLPIN	0x10
#define ST7735_SLPOUT	0x11
#define ST7735_PTLON	0x12
#define ST7735_NORON	0x13
#define ST7735_INVOFF	0x20
#define ST7735_INVON	0x21
#define ST7735_DISPOFF	0x28
#define ST7735_DISPON	0x29
#define ST7735_CASET	0x2A
#define ST7735_RASET	0x2B
#define ST7735_RAMWR	0x2C
#define ST7735_RAMRD	0x2E
#define ST7735_COLMOD	0x3A
#define ST7735_MADCTL	0x36
#define ST7735_FRMCTR1	0xB1
#define ST7735_FRMCTR2	0xB2
#define ST7735_FRMCTR3	0xB3
#define ST7735_INVCTR	0xB4
#define ST7735_DISSET5	0xB6
#define ST7735_PWCTR1	0xC0
#define ST7735_PWCTR2	0xC1
#define ST7735_PWCTR3	0xC2
#define ST7735_PWCTR4	0xC3
#define ST7735_PWCTR5	0xC4
#define ST7735_VMCTR1	0xC5
#define ST7735_RDID1	0xDA
#define ST7735_RDID2	0xDB
#define ST7735_RDID3	0xDC
#define ST7735_RDID4	0xDD
#define ST7735_GMCTRP1	0xE0
#define ST7735_GMCTRN1	0xE1
#define ST7735_PWCTR6	0xFC



/* List of commands and parameters to init lcd */
static struct st7735_function st7735_cfg_script[] = {
	{ ST7735_START, ST7735_START},
	{ ST7735_CMD, ST7735_SWRESET},
	{ ST7735_DELAY, 150},
	{ ST7735_CMD, ST7735_SLPOUT},
	{ ST7735_DELAY, 500},
	{ ST7735_CMD, ST7735_FRMCTR1},
	{ ST7735_DATA, 0x01},
	{ ST7735_DATA, 0x2c},
	{ ST7735_DATA, 0x2d},
	{ ST7735_CMD, ST7735_FRMCTR2},
	{ ST7735_DATA, 0x01},
	{ ST7735_DATA, 0x2c},
	{ ST7735_DATA, 0x2d},
	{ ST7735_CMD, ST7735_FRMCTR3},
	{ ST7735_DATA, 0x01},
	{ ST7735_DATA, 0x2c},
	{ ST7735_DATA, 0x2d},
	{ ST7735_DATA, 0x01},
	{ ST7735_DATA, 0x2c},
	{ ST7735_DATA, 0x2d},
	{ ST7735_CMD, ST7735_INVCTR},
	{ ST7735_DATA, 0x07},
	{ ST7735_CMD, ST7735_PWCTR1},
	{ ST7735_DATA, 0xa2},
	{ ST7735_DATA, 0x02},
	{ ST7735_DATA, 0x84},
	{ ST7735_CMD, ST7735_PWCTR2},
	{ ST7735_DATA, 0xc5},
	{ ST7735_CMD, ST7735_PWCTR3},
	{ ST7735_DATA, 0x0a},
	{ ST7735_DATA, 0x00},
	{ ST7735_CMD, ST7735_PWCTR4},
	{ ST7735_DATA, 0x8a},
	{ ST7735_DATA, 0x2a},
	{ ST7735_CMD, ST7735_PWCTR5},
	{ ST7735_DATA, 0x8a},
	{ ST7735_DATA, 0xee},
	{ ST7735_CMD, ST7735_VMCTR1},
	{ ST7735_DATA, 0x0e},
	{ ST7735_CMD, ST7735_INVOFF},
	{ ST7735_CMD, ST7735_MADCTL},
	{ ST7735_DATA, 0xc8},
	{ ST7735_CMD, ST7735_COLMOD},
	{ ST7735_DATA, 0x05},
	{ ST7735_CMD, ST7735_CASET},
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x7f},
	{ ST7735_CMD, ST7735_RASET},
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x9f},
	{ ST7735_CMD, ST7735_GMCTRP1},
	{ ST7735_DATA, 0x02},
	{ ST7735_DATA, 0x1c},
	{ ST7735_DATA, 0x07},
	{ ST7735_DATA, 0x12},
	{ ST7735_DATA, 0x37},
	{ ST7735_DATA, 0x32},
	{ ST7735_DATA, 0x29},
	{ ST7735_DATA, 0x2d},
	{ ST7735_DATA, 0x29},
	{ ST7735_DATA, 0x25},
	{ ST7735_DATA, 0x2b},
	{ ST7735_DATA, 0x39},
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x01},
	{ ST7735_DATA, 0x03},
	{ ST7735_DATA, 0x10},
	{ ST7735_CMD, ST7735_GMCTRN1},
	{ ST7735_DATA, 0x03},
	{ ST7735_DATA, 0x1d},
	{ ST7735_DATA, 0x07},
	{ ST7735_DATA, 0x06},
	{ ST7735_DATA, 0x2e},
	{ ST7735_DATA, 0x2c},
	{ ST7735_DATA, 0x29},
	{ ST7735_DATA, 0x2d},
	{ ST7735_DATA, 0x2e},
	{ ST7735_DATA, 0x2e},
	{ ST7735_DATA, 0x37},
	{ ST7735_DATA, 0x3f},
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x00},
	{ ST7735_DATA, 0x02},
	{ ST7735_DATA, 0x10},
	{ ST7735_CMD, ST7735_DISPON},
	{ ST7735_DELAY, 100},
	{ ST7735_CMD, ST7735_NORON},
	{ ST7735_DELAY, 10},
	{ ST7735_END, ST7735_END},
};
