CFLAGS_lol.o := -DDEBUG
ifneq ($(KERNELRELEASE),)
	obj-m := lol.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif
