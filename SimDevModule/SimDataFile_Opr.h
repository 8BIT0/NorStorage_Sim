#ifndef __SIMDATAFILE_OPR_H
#define __SIMDATAFILE_OPR_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <libgen.h>

typedef struct
{
    uint32_t size;
    uint8_t *p_buf;
    
    char *file_name;
    char *simdata_path_str;

    FILE *simdata_file;
    DIR *simdata_dir;

    void *(*malloc)(size_t size);
    void (*free)(void *ptr);
} SimDataFileObj_TypeDef;

typedef struct
{
    uint32_t size;
    uint8_t *p_buf;
} SimDataFileStream_TypeDef;

typedef struct
{
    bool (*create)(SimDataFileObj_TypeDef *data_obj, const char *app_path, const char *file_n, uint8_t mb_size);

    uint16_t (*write)(SimDataFileObj_TypeDef *data_obj, uint32_t data_addr, uint8_t *p_data, uint16_t size);
    uint16_t (*read)(SimDataFileObj_TypeDef *data_obj, uint32_t data_addr, uint8_t *p_data, uint16_t size);

    SimDataFileStream_TypeDef (*dump_file)(SimDataFileObj_TypeDef *data_obj);
} SimDataFile_TypeDef;

extern SimDataFile_TypeDef SimDataFile;

#endif
