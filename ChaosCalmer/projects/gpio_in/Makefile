TOOLCHAIN=/home/karolina/Documents/CrossCompile/Oolitev1/staging_dir/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33.2/
CC=$(TOOLCHAIN)/bin/mips-openwrt-linux-gcc
LD=$(TOOLCHAIN)/bin/mips-openwrt-linux-uclibc-ld
INCPATH=.
LDFLAGS=-lstdc++

gpio_in:
	$(CC) -I$(INCPATH) $(LDFLAGS) ../../library/src/gpio.c main.c -o gpio_in -pedantic-errors

all: gpio_in
