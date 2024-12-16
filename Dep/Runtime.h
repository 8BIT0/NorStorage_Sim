#ifndef __RUNTIME_H
#define __RUNTIME_H

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

#define MS_PER_S    1000
#define US_PER_MS   1000

typedef uint32_t SysTime_t;

typedef struct
{
    bool (*init)(void);
    void (*tick)(void);
    SysTime_t (*get_ms)(void);
} Runtime_TypeDef;

extern Runtime_TypeDef Runtime;

#endif
