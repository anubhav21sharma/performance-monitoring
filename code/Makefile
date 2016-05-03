obj-m += finalModule.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo dmesg -C
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
