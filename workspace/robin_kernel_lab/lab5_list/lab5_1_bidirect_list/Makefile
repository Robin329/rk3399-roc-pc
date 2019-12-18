BASEINCLUDE ?= /labs/linux-lab/output/aarch64/linux-v5.3.9-virt

list-objs := bidirect_list.o
obj-m := list.o
all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -rf *.ko;
