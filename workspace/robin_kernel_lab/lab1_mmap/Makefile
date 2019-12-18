ARCH=arm64
CROSS_COMPILE=aarch64-linux-gnu-

BASEINCLUDE ?= /labs/linux-lab/output/aarch64/linux-v5.3.9-virt
#BASEINCLUDE ?= /lib/modules/`uname -r`/build

mydevdemo_mmap-objs := mydev_mmap.o 

obj-m	:=   mydevdemo_mmap.o
all : 
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;


