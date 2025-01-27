#ifndef __UTIL_H
#define __UTIL_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define Kb   * 1024
#define Mb   * (1024 Kb)

void Debug_Print(const char *tag, const char *stage, const char *fmt, ...);
int16_t Common_CRC16(const uint8_t *pBuf, const uint32_t len);

#endif
