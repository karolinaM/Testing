i2cset -y 0 0x1a 0x1e 0x00 i
i2cset -y 0 0x1a 0x01 0x80 i
i2cset -y 0 0x1a 0x03 0x80 i
i2cset -y 0 0x1a 0x05 0xf0 i
i2cset -y 0 0x1a 0x07 0xf0 i
i2cset -y 0 0x1a 0x08 0xd0 i
i2cset -y 0 0x1a 0x0a 0x06 i
i2cset -y 0 0x1a 0x0c 0x00 i
i2cset -y 0 0x1a 0x0e 0x43 i
i2cset -y 0 0x1a 0x10 0x23 i
cat audio.wav > /dev/spidev1.0
