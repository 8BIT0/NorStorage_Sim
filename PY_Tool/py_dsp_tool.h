#ifndef __PY_DSP_TOOL_H
#define __PY_DSP_TOOL_H

#include <stdbool.h>
#include <string.h>
#include <stdint.h>

typedef struct
{
    bool (*init)(void);
    bool (*update)(uint8_t *p_data, uint16_t size);
    bool (*clear_all)(void);
} PyDsp_TypeDef;

extern PyDsp_TypeDef PY_Visualize;

#endif