/*
 * Library: wavfile.h
 *
 * Karolina Majstrovic
 * 2016
 * 
 */

#ifndef MYWAVFILE_H_
#define MYWAVFILE_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <byteswap.h>

#define WAV_SIZE            (unsigned int)4  // little
#define WAV_FORMAT          (unsigned int)8  // big
#define WAV_NUMBER_CHANNELS (unsigned int)22 // little
#define WAV_SAMPLE_RATE     (unsigned int)24 // little
#define WAV_BLOCK_ALIGN     (unsigned int)32 // little
#define WAV_BITS_PER_SAMPLE (unsigned int)34 // little
#define WAV_SUB_CHUNK_SIZE  (unsigned int)40 // little
#define WAV_DATA            (unsigned int)44 // little

#define WM8731_ADDRESS              0x1A       // WM8731 chip address on I2C bus
#define WM8731_REG_LLINE_IN         0x00       // Left Channel Line Input Volume Control
#define WM8731_REG_RLINE_IN         0x01       // Right Channel Line Input Volume Control
#define WM8731_REG_LHPHONE_OUT      0x02       // Left Channel Headphone Output Volume Control
#define WM8731_REG_RHPHONE_OUT      0x03       // Right Channel Headphone Output Volume Control
#define WM8731_REG_ANALOG_PATH      0x04       // Analog Audio Path Control
#define WM8731_REG_DIGITAL_PATH     0x05       // Digital Audio Path Control
#define WM8731_REG_PDOWN_CTRL       0x06       // Power Down Control Register
#define WM8731_REG_DIGITAL_IF       0x07       // Digital Audio Interface Format
#define WM8731_REG_SAMPLING_CTRL    0x08       // Sampling Control Register
#define WM8731_REG_ACTIVE_CTRL      0x09       // Active Control
#define WM8731_REG_RESET            0x0F       // Reset register

/**************************************************************************************************
* WM8731 sound chip constants (for default set up)
**************************************************************************************************/
#define _WM8731_left_lineIn        0x0180     // Mic settings: Enable mute, Enable simultaneous load to left and right channels
#define _WM8731_Right_lineIn       0x0180     // Mic settings: Enable mute, Enable simultaneous load to left and right channels
#define _WM8731_Left_hp            0x01F0     // Headphone settings : -9dB output
#define _WM8731_Right_hp           0x01F0     // Headphone settings : -9dB output
#define _WM8731_AnalogAudio        0xD0       // DAC Select
#define _WM8731_DigitalAudio       0x06
#define _WM8731_power              0x00       // Disable Power down
#define _WM8731_DAIF               0x43       // Enable Master Mode and 32bit data
#define _WM8731_Sampling           0x23       // 44100Hz
#define _WM8731_Activate           0x01       // Module is ON
#define _WM8731_Deactivate         0x00       // Module is OFF
#define _WM8731_Reset              0x00       // Reset value

void WM8731Cmd(int address, unsigned int cmd);

unsigned int ConvertToInt(char *buf, int n);
void WavInfo(int fd);
void WavPlay(int fd);
void WavInitI2C(); 

#endif
