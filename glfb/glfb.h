/* display parameters */
struct glfb_lcd_par {
	struct spi_device *spi;
	u16 height;
	u16 width;
	u16 bpp;
	u16 *ssbuf;
	int spi_mode;
	int rst;
	int dc;
	int cs;
	u32 speed;
	const s16 *init_sequence;	
};

/* Init script commands */
enum glfb_cmd {
	glfb_START,
	glfb_END,
	glfb_CMD,
	glfb_DATA,
	glfb_DELAY
};


int glfb_probe(struct spi_device *spi);
int glfb_remove(struct spi_device *spi);
/* 
static int glfb_init_display(void);
static void glfb_reset(void);
static int glfb_write(u8 data);
static void glfb_write_data(u8 data);
static int glfb_write_data_buf(u8 *txbuf, int size);
static void glfb_write_cmd(u8 data);
static void glfb_run_cfg_script(void);
static void glfb_set_addr_win(int xs, int ys, int xe, int ye);
*/

//int glfb_print(u16 *buffer, size_t size); 

#define GLFB_INIT(NAME, COPM) 							\
										\
static const struct of_device_id glfb_of_device_id[] = {			\
	{.compatible = COPM},							\
	{},									\
};										\
										\
MODULE_DEVICE_TABLE(of, glfb_of_device_id);					\
										\
static int glfb_probe_common(struct spi_device *spi)				\
{										\
	pr_info("glfb_probe_common\n");						\
	glfb_par.height = HEIGHT,						\
	glfb_par.width = WIDTH,							\
	glfb_par.bpp = BPP,							\
	glfb_par.rst = RST_GPIO,						\
	glfb_par.dc = DC_GPIO,							\
	glfb_par.cs = CS_GPIO;							\
	glfb_par.speed = SPI_BUS_SPEED;						\
	glfb_par.init_sequence = default_init_sequence;	 			\
	return glfb_probe(spi);							\
}										\
static int glfb_remove_common(struct spi_device *spi)				\
{										\
	return glfb_remove(spi);						\
}										\
										\
struct spi_driver glfb_driver = {						\
	.probe = glfb_probe_common,						\
	.remove = glfb_remove_common,						\
	.driver = {								\
		.name = NAME,							\
		.owner = THIS_MODULE,						\
		.of_match_table = glfb_of_device_id,				\
	},									\
};										\
										\
static int __init glfb_init(void)						\
{										\
	pr_info("glfb_init\n");							\
	return spi_register_driver(&glfb_driver);				\
}										\
										\
static void __exit glfb_exit(void)						\
{										\ 
	spi_unregister_driver(&glfb_driver);					\
}										\
										\
module_init(glfb_init);								\
module_exit(glfb_exit);																	
			 	
MODULE_DESCRIPTION("FB driver for glfb display controller");
MODULE_AUTHOR("Yurii Klysko");
MODULE_LICENSE("GPL");	 




