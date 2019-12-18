BASEINCLUDE ?= /labs/linux-lab/output/aarch64/linux-v5.3.9-virt

trace-objs := trace_demo.o
obj-m := trace.o
all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -rf *.ko;