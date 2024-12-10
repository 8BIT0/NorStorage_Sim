#include "SimDataFile_Opr.h"

#define SimDataFile_Folder  "./SimData/"
#define SimDataFile_Extend  ".sim"

#ifndef Kb
#define Kb(x)  (x * 1024)
#endif

#ifndef Mb
#define Mb(x)  (x * Kb(1024))
#endif

/* internal function */
static bool SimDataFile_CreateFolder(const char *path);

/* external function */
static bool SimDataFile_Create(SimDataFileObj_TypeDef *data_obj, const char *file_n, uint8_t mb_size);
static bool SimDataFile_Search(SimDataFileObj_TypeDef *data_obj, const char *file_n);
static SimDataFileStream_TypeDef SimDataFile_Dump(SimDataFileObj_TypeDef *data_obj);

SimDataFile_TypeDef SimDataFile = {
    .create = SimDataFile_Create,
    .search = SimDataFile_Search,
};

static bool SimDataFile_Create(SimDataFileObj_TypeDef *data_obj, const char *file_n, uint8_t mb_size)
{
    uint16_t file_name_size = 0;
    uint32_t write_size = 0;

    if ((data_obj == NULL) || (file_n == NULL) || (strlen(file_n) == 0) || \
        (mb_size == 0) || (data_obj->malloc == NULL) || (data_obj->free == NULL))
        return false;

    file_name_size = strlen(SimDataFile_Folder) + strlen(file_n) + strlen(SimDataFile_Extend) + 1;
    data_obj->p_buf = data_obj->malloc(file_name_size);
    data_obj->p_file = NULL;
    if (data_obj->p_buf == NULL)
    {
        data_obj->free(data_obj->p_buf);
        return false;
    }
    memset(data_obj->p_buf, '\0', file_name_size);
    sprintf(data_obj->p_buf, "%s%s%s", SimDataFile_Folder, file_n, SimDataFile_Extend);
    data_obj->size = Mb(mb_size);
    
    data_obj->p_file = fopen((const char *)data_obj->p_buf, "w+b");
    data_obj->free(data_obj->p_buf);
    data_obj->p_buf = NULL;

    if (data_obj->p_file == NULL)
        return false;

    data_obj->p_buf = data_obj->malloc(data_obj->size);
    if (data_obj->p_buf == NULL)
    {
        data_obj->free(data_obj->p_buf);
        return false;
    }
    memset(data_obj->p_buf, 0xFF, data_obj->size);

    if ((fwrite(data_obj->p_buf, sizeof(uint8_t), data_obj->size, data_obj->p_file) <= 0) || \
        (fclose(data_obj->p_file) <= 0))
        return false;

    memset(data_obj->p_buf, 0x00, data_obj->size);
    return true;
}

static bool SimDataFile_CreateFolder(const char *path)
{
    /* get app path */
    if ((path == NULL) || (strlen(path) == 0))
        return false;

    return false;
}

static bool SimDataFile_Search(SimDataFileObj_TypeDef *data_obj, const char *file_n)
{
    if ((data_obj == NULL) || (file_n == NULL))
        return false;

    /* search for SimData folder */

    /* search in SimData folder */

    return true;
}

static uint16_t SimDataFile_WriteSize(SimDataFileObj_TypeDef *data_obj, uint32_t data_addr, uint8_t *p_data, uint16_t size)
{
    if ((data_obj == NULL) || \
        (data_obj->size == 0) || \
        (data_obj->p_file == NULL) || \
        (p_data == NULL) || (size == 0))
        return 0;

    /* open file */

    /* close file */

    return 0;
}

static uint16_t SimDataFile_ReadSize(SimDataFileObj_TypeDef *data_obj, uint32_t data_addr, uint8_t *p_data, uint16_t size)
{
    if ((data_obj == NULL) || \
        (data_obj->size == 0) || \
        (data_obj->p_file == NULL) || \
        (p_data == NULL) || (size == 0))
        return 0;

    /* open file */

    /* close file */
    
    return 0;
}

static SimDataFileStream_TypeDef SimDataFile_Dump(SimDataFileObj_TypeDef *data_obj)
{
    SimDataFileStream_TypeDef stream_tmp;
    memset(&stream_tmp, 0, sizeof(SimDataFileStream_TypeDef));

    if ((data_obj != NULL) && (data_obj->p_buf != NULL) && (data_obj->size != 0) && (data_obj->p_file != NULL))
    {

    }

    return stream_tmp;
}
