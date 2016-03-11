/*
 * Library: gpioh
 *
 * Karolina Majstrovic
 * 2016
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define gpioDIRECTION_IN  (int)0
#define gpioDIRECTION_OUT (int)1

void gpioExport(int gpio);
int  gpioDirection(int gpio, int direction);
void gpioSet(int gpio, int value);
char gpioRead(int gpio);
void gpioInterruptConfig(int gpio);
