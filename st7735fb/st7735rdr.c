#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/string.h>
#include "font5x7.h"
#include <linux/delay.h>
#include "st7735fb.h"
/*
struct st7735_size {
	u16 height;
	u16 width;
	u16 bpp;
};
extern struct st7735_size st7735_s; 
extern int st7735fb_print(u16 *buffer, size_t size);
*/
char string[] = "If it is impractical to add a top-level kbuild file, you can assign a space separated list of files to KBUILD_EXTRA_SYMBOLS in your build file. These files will be loaded by modpost during the initialization of its symbol tables.";

u16 *pix_buf;
u32 pix_buf_size;

static u32 font_w = FONT_CHAR_WIDTH;
static u32 font_h = FONT_CHAR_HEIGHT;
static u8 *char_buf;
static u32 char_buf_size;
static u32 char_buf_h, char_buf_w;

static u16 font_color = 0xffff;
static u16 screen_color = 0x0000;

static void string_to_pix(void)
{
	u32 ipix = 0;
	if (strlen(string) < char_buf_size)
		memcpy(char_buf, string, strlen(string));
	else
		memcpy(char_buf, string, char_buf_size);
	pr_info("char_buf_size: %u\n", char_buf_size);
	 
		
	for (int i = 0; i < char_buf_h; i++)
	{
		for (int j = 0; j < font_h; j++)
		{
			for (int k = 0; k < st7735_s.width; k++)
			{ 
				u8 letter = char_buf[i * char_buf_w + k / font_w] - 32;
				/*if ((letter < 32) && (letter > 127))
					letter = letter - 32;
				else
					letter = 0;
					*/
				if (font[letter][k % font_w] & ((u8) 0x01 << j))
					pix_buf[ipix] = font_color;
				else
					pix_buf[ipix] = screen_color;
				
				if ((i == 0) && (k < 3 * font_w)) {
					pr_info("k: %u pix: %d ", k, pix_buf[ipix]);
				}
				ipix++;
					//pix_buf[k + j * st7735_s.width + i * st7735_s.width * font_h] = font_color; 
			}
		}
		
		
	}
	
	
	
	st7735fb_print(pix_buf, pix_buf_size);
	
}

static int __init st7735rdr_init(void) 
{
	pix_buf_size =  st7735_s.width * st7735_s.height * st7735_s.bpp / 8;
	pix_buf = vzalloc(pix_buf_size);
	memset(pix_buf, 0x00, pix_buf_size);
	if (!pix_buf) 
		return -ENOMEM;	
	
	char_buf_h = st7735_s.height / FONT_CHAR_HEIGHT;
	char_buf_w = st7735_s.width / FONT_CHAR_WIDTH;
	char_buf_size = char_buf_h * char_buf_w;
	char_buf = vzalloc(char_buf_size);
	string_to_pix();
	return 0;
}

static void __exit st7735rdr_exit(void) {
	pr_info("End fo the world!\n");
}


module_init(st7735rdr_init);
module_exit(st7735rdr_exit);
MODULE_AUTHOR("YK");
MODULE_LICENSE("GPL");
