#ifndef __SIMDATAFILE_OPR_H
#define __SIMDATAFILE_OPR_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <libgen.h>
#include "SysCommon.h"

typedef struct
{
    uint32_t size;
    uint8_t *p_buf;
    
    char *file_name;
    char *simdata_path_str;

    FILE *simdata_file;
    DIR *simdata_dir;

    void *(*malloc)(uint32_t size);
    void (*free)(void *ptr);
} SimDataFileObj_TypeDef;

typedef struct
{
    bool (*create)(SimDataFileObj_TypeDef *data_obj, const char *app_path, const char *file_n, uint32_t mb_size);

    uint16_t (*write)(SimDataFileObj_TypeDef *data_obj, uint32_t addr, uint8_t *p_data, uint16_t size);
    uint16_t (*read)(SimDataFileObj_TypeDef *data_obj, uint32_t addr, uint8_t *p_data, uint16_t size);

    bool (*dump_file)(SimDataFileObj_TypeDef *data_obj, Stream_TypeDef *stream);
} SimDataFile_TypeDef;

extern SimDataFile_TypeDef SimDataFile;

#endif
