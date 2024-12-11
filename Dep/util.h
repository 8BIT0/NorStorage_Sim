#ifndef __UTIL_H
#define __UTIL_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define Kb(x)  (x * 1024)
#define Mb(x)  (x * Kb(1024))

void Debug_Print(const char *tag, const char *stage, const char *fmt, ...);

#endif
