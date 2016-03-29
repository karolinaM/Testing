TOOLCHAIN=/home/karolina/Documents/CrossCompile/Oolitev1/staging_dir/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33.2/
CC=$(TOOLCHAIN)/bin/mips-openwrt-linux-gcc
LD=$(TOOLCHAIN)/bin/mips-openwrt-linux-uclibc-ld
INCPATH=-I. -I/home/karolina/Documents/CrossCompile/Oolitev1/build_dir/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33.2/linux-3.3.8/include
LDFLAGS=-lstdc++

gpio_interrupt:
	$(CC) $(INCPATH) $(LDFLAGS) ../../library/src/gpio.c main.c -o gpio_interrupt

all: gpio_interrupt
