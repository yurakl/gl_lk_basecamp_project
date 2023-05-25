#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/string.h>
#include "font5x7.h"
#include <linux/delay.h>
#include "glfb.h" 
u16 *pix_buf;
u32 pix_buf_size;
u8 *char_buf;
u32 char_buf_size;
u32 char_buf_h, char_buf_w;
u16 font_color = 0xffff;
u16 screen_color = 0x0000;

extern struct glfb_lcd_par glfb_par; 
extern int glfb_print(u16 *buffer, size_t size);
u32 font_w = FONT_CHAR_WIDTH;
u32 font_h = FONT_CHAR_HEIGHT;

void glfb_print_string(char * string)
{
	u32 ipix = 0;
	static u32 position = 0;
	if (strlen(string) < char_buf_size)
		memcpy(char_buf, string, strlen(string));
	else
		memcpy(char_buf, string, char_buf_size);
	pr_info("char_buf_size: %u\n", char_buf_size);
	 
		
	//for (int i = position; i < char_buf_h; i++)
	for (int i = 0; i < char_buf_h; i++)
	{
		/*
		if (position == char_buf_h) {
			memcpy(char_buf, char_buf + char_buf_w, char_buf_size - char_buf_w);
			position--;
		}
		*/	
		for (int j = 0; j < font_h; j++)
		{
			for (int k = 0; k < glfb_par.width; k++)
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
	
	
	
	glfb_print(pix_buf, pix_buf_size);
	
}
EXPORT_SYMBOL(glfb_print_string);
void glfb_clean(void)
{
	memset((u16 *) pix_buf, screen_color, pix_buf_size);
	memset(char_buf, 0, char_buf_size);
	glfb_print(pix_buf, pix_buf_size);
	return;
}
EXPORT_SYMBOL(glfb_clean);

static int __init glfb_rdr_init(void) 
{
	pix_buf_size =  glfb_par.width * glfb_par.height * glfb_par.bpp / 8;
	pix_buf = vzalloc(pix_buf_size);
	memset(pix_buf, 0x00, pix_buf_size);
	if (!pix_buf) 
		return -ENOMEM;	
	
	char_buf_h = glfb_par.height / FONT_CHAR_HEIGHT;
	char_buf_w = glfb_par.width / FONT_CHAR_WIDTH;
	char_buf_size = char_buf_h * char_buf_w;
	char_buf = vzalloc(char_buf_size); 
	glfb_print_string("Wake up, Neo");
	return 0;
}

static void __exit glfb_rdr_exit(void) {
	pr_info("End fo the world!\n"); 
}


module_init(glfb_rdr_init);
module_exit(glfb_rdr_exit);
MODULE_AUTHOR("YK");
MODULE_LICENSE("GPL");
