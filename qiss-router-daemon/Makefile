obj-m += qstcp.o

# CCFLAGS = -nostartfiles

KBUILD_EXTRA_SYMBOLS=Module.symvers

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
