TOOLCHAIN=/home/karolina/Documents/CrossCompile/chaos/openwrt/staging_dir/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-0.9.33.2
CC=$(TOOLCHAIN)/bin/mips-openwrt-linux-gcc
LD=$(TOOLCHAIN)/bin/mips-openwrt-linux-uclibc-ld
INCPATH=.
LDFLAGS=-lstdc++

i2c_simple:
	$(CC) -I$(INCPATH) $(LDFLAGS) ../../library/src/gpio.c ../../library/src/wavfile.c main.c -o i2c_simple

all: i2c_simple
