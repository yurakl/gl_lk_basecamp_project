obj-m += glfb_st7735.o  glfb-rdr.o 
glfb_st7735-objs := st7735fb.o  glfb-core.o
ccflags-y := -std=gnu99

all:
	$(MAKE) -C  /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
