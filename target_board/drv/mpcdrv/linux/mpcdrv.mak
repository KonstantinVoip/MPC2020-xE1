CC=/opt/freescale/usr/local/gcc-4.3.74-eglibc-2.8.74-dp-2/powerpc-none-linux-gnuspe/bin/powerpc-none-linux-gnuspe-gcc
TARGET = mpcdrv
ARCH   = powerpc
MAKE   = make
KERNELINC := /usr/src/linux/include

# create driver from command line
ifeq ($(KERNELRELEASE),)
KERNELDIR := /usr/src/linux
PWD       := $(shell pwd)


#obj-m		  := $(TARGET).o


# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g 	# "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif


# compiler configuration
CFLAGS += $(DEBFLAGS) -I$(LDDINC) -I$(KERNELINC) -D__KERNEL__ -DMODULE \
	  -DUSE_B_DMA


default:
	
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD) modules
	@ chmod 777 .*.cmd
	@ chmod -R 777 ./.tmp_versions

	
else
#$(TARGET)-objs := drv_rel.o hifdrv.o \
#		  hifdrv_util.o dif.o nbuf.o \
#		  flash.o mpc82xx_pci.o pspan.o qspan.o
#obj-m		  := $(TARGET).o
endif


# clean up
clean:
	@ echo "* clean up *"
	rm -rf *.o *.ko \
	        .depend *.dep \
		.*.cmd \
		.*.o.d \
		*.mpcdrv.c \
		*.order \
		Module.symvers \
		.tmp_versions \
		*~ *.bck core
	@ echo


# create dependency file
depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend
    
#include dependency file
ifeq (.depend,$(wildcard .depend))
include .depend
endif	
	
	
#$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD) modules
#obj-m := $(TARGET).o
#obj-m  += mpcdrv.o
  