obj-m += simple_char_driver.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install:
	sudo insmod simple_char_driver.ko

uninstall:
	sudo rmmod simple_char_driver

test:
	sudo dmesg -c
	sudo insmod simple_char_driver.ko
	dmesg
	ls -la /dev/simple_char_dev
	sudo rmmod simple_char_driver
	dmesg

.PHONY: all clean install uninstall test
