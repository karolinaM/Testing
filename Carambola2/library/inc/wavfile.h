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


void WavInfo(int fd);
void WavPlay(int fd);

#endif