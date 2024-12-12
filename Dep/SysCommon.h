#ifndef __SYSCOMMON_H
#define __SYSCOMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
    uint32_t size;
    uint8_t *p_buf;
} Stream_TypeDef;

#endif
