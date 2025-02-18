#ifndef __PY_DSP_TOOL_H
#define __PY_DSP_TOOL_H

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "Callback_Dep/ui_callback.h"

typedef enum
{
    Opr_Idle = 0,
    Opr_Create_Item,
    Opr_Write_Item,
    Opr_Read_Item,
    Opr_Delete_Item,
} VisualOpera_Type_List;

typedef struct
{
    /* data */
    uint8_t Opr_Type;
    char *item_name;

    struct
    {
        /* data */
        uint8_t *data;
        uint16_t len;
    } stream;
} SimInput_TypeDef;

typedef struct
{
    bool (*init)(char *simfile_dir, char *file_name, uint32_t addr_offset);
    void (*update)(void);
} PyDsp_TypeDef;

extern PyDsp_TypeDef PY_Visualize;

#endif