#include "SimDataFile_Opr.h"
#include "util.h"

#define SimDataFile_Folder  "\\SimData"
#define SimDataFile_Extend  ".sim"
#define SimDataFile_MKDIR   "mkdir "

#define SIMDATA_TAG "SimData"
#define SIMDATA_PRINT(stage, fmt, ...) Debug_Print(SIMDATA_TAG, stage, fmt, ##__VA_ARGS__) 

/* internal function */
static bool SimDataFile_CreateFolder(SimDataFileObj_TypeDef *data_obj);

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
    {
        SIMDATA_PRINT("create", "Invalid parameter");
        return false;
    }

    /* check folder */
    if (!SimDataFile_CreateFolder(data_obj))
        return false;

    /* open folder */
    

    /* create sim data file */
    file_name_size = strlen(file_n) + strlen(SimDataFile_Extend);
    data_obj->p_buf = data_obj->malloc(file_name_size);
    data_obj->simdata_file = NULL;
    if (data_obj->p_buf == NULL)
    {
        SIMDATA_PRINT("create file", "Failed");
        data_obj->free(data_obj->p_buf);
        return false;
    }
    memset(data_obj->p_buf, '\0', file_name_size);

    sprintf(data_obj->p_buf, "%s%s%s", SimDataFile_Folder, file_n, SimDataFile_Extend);
    data_obj->size = Mb(mb_size); data_obj->simdata_file = fopen((const char *)data_obj->p_buf, "w+b");
    data_obj->free(data_obj->p_buf);
    data_obj->p_buf = NULL;

    if (data_obj->simdata_file == NULL)
        return false;

    data_obj->p_buf = data_obj->malloc(data_obj->size);
    if (data_obj->p_buf == NULL)
    {
        data_obj->free(data_obj->p_buf);
        return false;
    }
    memset(data_obj->p_buf, 0xFF, data_obj->size);

    if ((fwrite(data_obj->p_buf, sizeof(uint8_t), data_obj->size, data_obj->simdata_file) <= 0) || \
        (fclose(data_obj->simdata_file) <= 0))
        return false;
    /* create sim data file */

    memset(data_obj->p_buf, 0x00, data_obj->size);
    return true;
}

static bool SimDataFile_CreateFolder(SimDataFileObj_TypeDef *data_obj)
{
    char *path_str = NULL;
    char *path_offset = NULL;
    uint16_t path_len = 0;
    const char* app_dir = dirname(_pgmptr);
    int ret = 0;

    /* create sub folder in the same path of application*/
    /* create path name string */
    SIMDATA_PRINT("application path", "%s", app_dir);
    path_len = strlen(app_dir) + strlen(SimDataFile_Folder);
    data_obj->simdata_path = data_obj->malloc(path_len);
    path_len += strlen(SimDataFile_MKDIR);
    path_str = data_obj->malloc(path_len);
    if ((path_str == NULL) || (data_obj->simdata_path == NULL))
    {
        SIMDATA_PRINT("create path", "Failed");
        data_obj->free(path_str);
        data_obj->free(data_obj->simdata_path);
        return false;
    }
    memset(data_obj->simdata_path, '\0', path_len - strlen(SimDataFile_MKDIR));
    memset(path_str, '\0', path_len);
    
    path_offset = path_str + strlen(SimDataFile_MKDIR);
    strcpy(path_offset, app_dir);
    strcat(path_offset, SimDataFile_Folder);
    memcpy(path_str, SimDataFile_MKDIR, strlen(SimDataFile_MKDIR));
    strcpy(data_obj->simdata_path, path_offset);
    SIMDATA_PRINT("SimData path", "%s", data_obj->simdata_path);

    /* check folder */
    if ((data_obj->simdata_dir = opendir(path_offset)) == NULL)
    {
        /* create path string */
        SIMDATA_PRINT("create SimData path", "%s", path_str);
        if (system((const char *)path_str) != 0)
        {
            SIMDATA_PRINT("create SimData path", "Failed Error Code: %d", ret);
            data_obj->free(path_str);
            data_obj->free(data_obj->simdata_path);
            path_str = NULL;
            return false;
        }
        SIMDATA_PRINT("create SimData path", "Done");

        /* get folder object pointer */
        data_obj->simdata_dir = opendir(path_offset);
        if (data_obj->simdata_dir == NULL)
        {
            SIMDATA_PRINT("open SimData path", "Failed");
            data_obj->free(path_str);
            data_obj->free(data_obj->simdata_path);
            path_str = NULL;
            return false;
        }
    }
    else
        SIMDATA_PRINT("SimData path", "Already Exist");
    closedir(data_obj->simdata_dir);

    /* free path string */
    path_len = 0;
    data_obj->free(path_str);
    path_str = NULL;

    return true;
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
        (data_obj->simdata_file == NULL) || \
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
        (data_obj->simdata_file == NULL) || \
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

    if ((data_obj != NULL) && (data_obj->p_buf != NULL) && (data_obj->size != 0) && (data_obj->simdata_file != NULL))
    {

    }

    return stream_tmp;
}
