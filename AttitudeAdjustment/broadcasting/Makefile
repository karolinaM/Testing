TOOLCHAIN=/home/karolina/Documents/CrossCompile/the_best_version/openwrt-15.05/staging_dir/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-0.9.33.2/
CC=$(TOOLCHAIN)/bin/mips-openwrt-linux-gcc
LD=$(TOOLCHAIN)/bin/mips-openwrt-linux-uclibc-ld
INCPATH=.
LDFLAGS=-lstdc++

audio_server:
	$(CC) -I$(INCPATH) $(LDFLAGS) audio_server.c -o audio_server

all: audio_server
