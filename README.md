#gl_lk_basecamp_project
Final project for Linux Kernel Base Camp . BMP + LCD

st7735fb is a module without linux framebuffer support. The inter-module API: 
struct st7735_size - includes screen size and coding format (bit per pixel). 
int st7735fb_print(u16 *buffer, size_t size) - function, which recive a buffer to print.
