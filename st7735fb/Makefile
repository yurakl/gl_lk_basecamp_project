obj-m += st7735fb.o rpi_st7735fb.o  
all:
	$(MAKE) -C  /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
