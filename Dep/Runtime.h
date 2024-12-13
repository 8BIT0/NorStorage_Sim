#ifndef __RUNTIME_H
#define __RUNTIME_H

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

typedef uint64_t SysTime_t;

typedef struct
{
    bool (*init)(void);
    void (*tick)(void);
    SysTime_t (*get_ms)(void);
} Runtime_TypeDef;

#endif
