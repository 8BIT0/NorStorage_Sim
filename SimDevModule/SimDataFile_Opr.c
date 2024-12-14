#include "SimDataFile_Opr.h"
#include "util.h"

#if defined WIN
#define Folder_Split  "\\"
#elif defined MAC || defined LINUX
#define Folder_Split  "/"
#endif
#define SimDataFile_Folder  "SimData"
#define SimDataFile_Extend  ".sim"
#define SimDataFile_MKDIR   "mkdir "

#define SIMDATA_TAG "SimData"
#define SIMDATA_PRINT(stage, fmt, ...) Debug_Print(SIMDATA_TAG, stage, fmt, ##__VA_ARGS__) 

/* internal function */
static bool SimDataFile_CreateFolder(SimDataFileObj_TypeDef *data_obj, const char *app_path);
static bool SimDataFile_CheckFile(SimDataFileObj_TypeDef *data_obj, char *file_name);
static void SimDataFile_Free(SimDataFileObj_TypeDef *data_obj);
static uint32_t SimDataFile_GetSize(SimDataFileObj_TypeDef *data_obj);

/* external function */
static bool SimDataFile_Create(SimDataFileObj_TypeDef *data_obj, const char *app_path, const char *file_n, uint8_t mb_size);
static uint16_t SimDataFile_WriteSize(SimDataFileObj_TypeDef *data_obj, uint8_t *p_data, uint16_t size);
static bool SimDataFile_Dump(SimDataFileObj_TypeDef *data_obj, Stream_TypeDef *stream);
static uint16_t SimDataFile_ReadSize(SimDataFileObj_TypeDef *data_obj, uint8_t *p_data, uint16_t size);
static bool SimDataFile_Dump(SimDataFileObj_TypeDef *data_obj, Stream_TypeDef *stream);

SimDataFile_TypeDef SimDataFile = {
    .create = SimDataFile_Create,
    .write = SimDataFile_WriteSize,
    .read = SimDataFile_ReadSize,
};

static void SimDataFile_Free(SimDataFileObj_TypeDef *data_obj)
{
    if (data_obj == NULL)
        return;

    if (data_obj->p_buf)
    {
        data_obj->free(data_obj->p_buf);
        data_obj->p_buf = NULL;
    }

    if (data_obj->simdata_path_str)
    {
        data_obj->free(data_obj->simdata_path_str);
        data_obj->simdata_path_str = NULL;
    }

    if (data_obj->file_name)
    {
        data_obj->free(data_obj->file_name);
        data_obj->file_name = NULL;
    }
}

static bool SimDataFile_Create(SimDataFileObj_TypeDef *data_obj, const char *app_path, const char *file_n, uint8_t mb_size)
{
    uint16_t file_name_size = 0;
    uint32_t write_size = 0;
    int ret = 0;

    if ((data_obj == NULL) || (file_n == NULL) || (strlen(file_n) == 0) || \
        (mb_size == 0) || (data_obj->malloc == NULL) || (data_obj->free == NULL))
    {
        SIMDATA_PRINT("create SimData", "Invalid parameter");
        return false;
    }
    data_obj->size = (mb_size Mb);

    /* check folder */
    if (!SimDataFile_CreateFolder(data_obj, app_path))
        return false;

    if (data_obj->p_buf != NULL)
    {
        SIMDATA_PRINT("Create SimData", "p_buf Error");
        return false;
    }

    /* create sim data file */
    SIMDATA_PRINT("create SimData file", "start");
    file_name_size = strlen(file_n) + strlen(SimDataFile_Extend) + strlen(data_obj->simdata_path_str);
    data_obj->p_buf = data_obj->malloc(file_name_size);
    data_obj->simdata_file = NULL;
    if (data_obj->p_buf == NULL)
    {
        SIMDATA_PRINT("create SimData file", "Failed");
        SimDataFile_Free(data_obj);
        return false;
    }
    memset(data_obj->p_buf, '\0', file_name_size);

    /* check file in selected folder */
    sprintf((char *)data_obj->p_buf, "%s%s", file_n, SimDataFile_Extend);
    if (SimDataFile_CheckFile(data_obj, (char *)data_obj->p_buf))
    {
        SIMDATA_PRINT("Create SimData file", "file pointer 0x%08X", data_obj->simdata_file);
        data_obj->free(data_obj->p_buf);
        data_obj->p_buf = NULL;
        return true;
    }
    memset(data_obj->p_buf, '\0', file_name_size);

    /* create sim data file */
    sprintf((char *)data_obj->p_buf, "%s%s%s%s", data_obj->simdata_path_str, Folder_Split, file_n, SimDataFile_Extend);
    SIMDATA_PRINT("create SimData file", "%s", data_obj->p_buf);
    data_obj->simdata_file = fopen((const char *)data_obj->p_buf, "w+b");
    data_obj->free(data_obj->p_buf);
    data_obj->p_buf = NULL;

    if (data_obj->simdata_file == NULL)
    {
        SimDataFile_Free(data_obj);
        return false;
    }

    data_obj->p_buf = data_obj->malloc(data_obj->size);
    if (data_obj->p_buf == NULL)
    {
        SimDataFile_Free(data_obj);
        return false;
    }
    memset(data_obj->p_buf, 0xFF, data_obj->size);

    if (fwrite(data_obj->p_buf, sizeof(uint8_t), data_obj->size, data_obj->simdata_file) <= 0)
    {
        SIMDATA_PRINT("SimData file", "Write failed");
        SimDataFile_Free(data_obj);
        return false;
    }

    if ((ret = fclose(data_obj->simdata_file)) != 0)
    {
        SIMDATA_PRINT("SimData file", "Close failed error code: %d", ret);
        SimDataFile_Free(data_obj);
        return false;
    }

    SIMDATA_PRINT("Create SimData file", "Done");
    SIMDATA_PRINT("Create SimData file", "file pointer 0x%08X", data_obj->simdata_file);
    memset(data_obj->p_buf, 0x00, data_obj->size);
    data_obj->free(data_obj->p_buf);
    data_obj->p_buf = NULL;
    return true;
}

static bool SimDataFile_CreateFolder(SimDataFileObj_TypeDef *data_obj, const char *app_path)
{
    char *path_offset = data_obj->simdata_path_str;
    uint16_t path_len = 0;
    const char* app_dir = NULL;
    int ret = 0;

    if (data_obj->simdata_path_str == NULL)
    {
        app_dir = dirname((char *)app_path);
        path_offset = NULL;

        /* create sub folder in the same path of application*/
        /* create path name string */
        SIMDATA_PRINT("application path", "%s", app_dir);
        path_len = strlen(app_dir) + strlen(Folder_Split) + strlen(SimDataFile_Folder);
        data_obj->simdata_path_str = data_obj->malloc(path_len);
        path_len += strlen(SimDataFile_MKDIR);
        data_obj->p_buf = data_obj->malloc(path_len);
        if ((data_obj->p_buf == NULL) || (data_obj->simdata_path_str == NULL))
        {
            SIMDATA_PRINT("create path", "Failed");
            SimDataFile_Free(data_obj);
            return false;
        }
        memset(data_obj->simdata_path_str, '\0', path_len - strlen(SimDataFile_MKDIR));
        memset(data_obj->p_buf, '\0', path_len);
        
        path_offset = (char *)(data_obj->p_buf + strlen(SimDataFile_MKDIR));
        strcpy(path_offset, app_dir);
        strcat(path_offset, Folder_Split);
        strcat(path_offset, SimDataFile_Folder);
        memcpy(data_obj->p_buf, SimDataFile_MKDIR, strlen(SimDataFile_MKDIR));
        strcpy(data_obj->simdata_path_str, path_offset);
        SIMDATA_PRINT("SimData path", "%s", data_obj->simdata_path_str);
    }
    
    /* check folder */
    if ((data_obj->simdata_dir = opendir(path_offset)) == NULL)
    {
        /* create path string */
        SIMDATA_PRINT("create SimData path", "%s", data_obj->p_buf);
        if (system((const char *)data_obj->p_buf) != 0)
        {
            SIMDATA_PRINT("create SimData path", "Failed Error Code: %d", ret);
            SimDataFile_Free(data_obj);
            return false;
        }
        SIMDATA_PRINT("create SimData path", "Done");

        /* get folder object pointer */
        data_obj->simdata_dir = opendir(path_offset);
        if (data_obj->simdata_dir == NULL)
        {
            SIMDATA_PRINT("open SimData path", "Failed");
            SimDataFile_Free(data_obj);
            return false;
        }
    }
    else
        SIMDATA_PRINT("SimData path", "Already Exist");
    closedir(data_obj->simdata_dir);

    /* free path string */
    data_obj->simdata_dir = NULL;
    path_len = 0;
    data_obj->free(data_obj->p_buf);
    data_obj->p_buf = NULL;

    return true;
}

static bool SimDataFile_CheckFile(SimDataFileObj_TypeDef *data_obj, char *file_name)
{
    struct dirent *folder_item = NULL;

    if ((file_name == NULL) || (strlen(file_name) == 0) || (data_obj->malloc == NULL) || (data_obj->free == NULL))
    {
        SIMDATA_PRINT("check file", "Invalid file name");
        return false;
    }
    SIMDATA_PRINT("check SimData file", "%s", file_name);

    if (data_obj->file_name == NULL)
    {
        data_obj->file_name = data_obj->malloc(strlen(file_name));
        if (data_obj->file_name == NULL)
        {
            SIMDATA_PRINT("file name", "Set failed");
            SimDataFile_Free(data_obj);
            return false;
        }
        memset(data_obj->file_name, '\0', strlen(file_name));
        strcpy(data_obj->file_name, file_name);
    }

    /* check folder search for all sim file */
    data_obj->simdata_dir = opendir(data_obj->simdata_path_str);
    if (data_obj->simdata_dir == NULL)
    {
        SIMDATA_PRINT("check SimData file", "Open folder %s failed", data_obj->simdata_path_str);
        return false;
    }

    SIMDATA_PRINT("check SimData file", "Open folder %s done", data_obj->simdata_path_str);
    while ((folder_item = readdir(data_obj->simdata_dir)) != NULL)
    {
        if (folder_item->d_namlen == 0)
            continue;    
    
        SIMDATA_PRINT("check SimData file", "%s", folder_item->d_name);
        if (memcmp(folder_item->d_name, file_name, strlen(file_name)) == 0)
        {
            SIMDATA_PRINT("check SimData file", "File name matched");
            return true;
        }
    }
    SIMDATA_PRINT("check SimData file", "File name unmatched");

    return false;
}

static uint16_t SimDataFile_WriteSize(SimDataFileObj_TypeDef *data_obj, uint8_t *p_data, uint16_t size)
{
    char name_buf[strlen(data_obj->simdata_path_str) + strlen(data_obj->file_name) + strlen(Folder_Split)];

    if ((data_obj == NULL) || \
        (data_obj->simdata_path_str == NULL) || \
        (data_obj->file_name == NULL) || \
        (p_data == NULL) || (size == 0))
        return 0;

    memset(name_buf, '\0', (strlen(data_obj->simdata_path_str) + strlen(data_obj->file_name)));
    strcpy(name_buf, data_obj->simdata_path_str);
    strcat(name_buf, Folder_Split);
    strcat(name_buf, data_obj->file_name);

    /* open file */
    if ((data_obj->simdata_file = fopen(name_buf, "rb+")) == NULL)
    {
        SIMDATA_PRINT("write file", "Open file %s failed", data_obj->file_name);
        data_obj->opr_addr = 0;
        return 0;
    }

    if (fseek(data_obj->simdata_file, data_obj->opr_addr, 0) != 0)
    {
        SIMDATA_PRINT("write file", "seek to %d failed", data_obj->opr_addr);
        data_obj->opr_addr = 0;
        return 0;
    }

    data_obj->opr_addr = 0;
    /* write to file */
    if (fwrite(p_data, sizeof(uint8_t), size, data_obj->simdata_file) <= 0)
    {
        SIMDATA_PRINT("write file", "failed");
        return 0;
    }

    /* close file */
    if (fseek(data_obj->simdata_file, 0, 0) != 0)
    {
        SIMDATA_PRINT("write file", "seek to start failed");
        return 0;
    }

    if (fclose(data_obj->simdata_file) != 0)
    {
        SIMDATA_PRINT("write file", "Close file failed");
        return 0;
    }

    return size;
}

static uint16_t SimDataFile_ReadSize(SimDataFileObj_TypeDef *data_obj, uint8_t *p_data, uint16_t size)
{
    char name_buf[strlen(data_obj->simdata_path_str) + strlen(data_obj->file_name) + strlen(Folder_Split)];
    
    if ((data_obj == NULL) || \
        (data_obj->simdata_path_str == NULL) || \
        (data_obj->file_name == NULL) || \
        (p_data == NULL) || (size == 0))
        return 0;
    
    memset(name_buf, '\0', (strlen(data_obj->simdata_path_str) + strlen(data_obj->file_name)));
    strcpy(name_buf, data_obj->simdata_path_str);
    strcat(name_buf, Folder_Split);
    strcat(name_buf, data_obj->file_name);

    /* open file */
    if ((data_obj->simdata_file = fopen(name_buf, "rb+")) == NULL)
    {
        SIMDATA_PRINT("write file", "Open file %s failed", data_obj->file_name);
        data_obj->opr_addr = 0;
        return 0;
    }

    if (fseek(data_obj->simdata_file, data_obj->opr_addr, 0) != 0)
    {
        SIMDATA_PRINT("write file", "seek to %d failed", data_obj->opr_addr);
        data_obj->opr_addr = 0;
        return 0;
    }

    data_obj->opr_addr = 0;
    /* read file */
    if (fread(p_data, sizeof(uint8_t), size, data_obj->simdata_file) <= 0)
    {
        SIMDATA_PRINT("read file", "failed");
        return 0;
    }

    /* close file */
   if (fseek(data_obj->simdata_file, 0, 0) != 0)
    {
        SIMDATA_PRINT("write file", "seek to start failed");
        return 0;
    }

    if (fclose(data_obj->simdata_file) != 0)
    {
        SIMDATA_PRINT("write file", "Close file failed");
        return 0;
    }

    return size;
}

static uint32_t SimDataFile_GetSize(SimDataFileObj_TypeDef *data_obj)
{
    char name_buf[strlen(data_obj->simdata_path_str) + strlen(data_obj->file_name) + strlen(Folder_Split)];
    uint32_t file_size = 0;

    if ((data_obj == NULL) || \
        (data_obj->file_name == NULL) || \
        (strlen(data_obj->file_name) == 0) || \
        (data_obj->simdata_path_str == NULL) || \
        (strlen(data_obj->simdata_path_str) == 0))
        return 0;

    memset(name_buf, '\0', (strlen(data_obj->simdata_path_str) + strlen(data_obj->file_name)));
    strcpy(name_buf, data_obj->simdata_path_str);
    strcat(name_buf, Folder_Split);
    strcat(name_buf, data_obj->file_name);

    /* open file */
    if ((data_obj->simdata_file = fopen(name_buf, "rb+")) == NULL)
    {
        SIMDATA_PRINT("write file", "Open file %s failed", data_obj->file_name);
        return 0;
    }

    if (fseek(data_obj->simdata_file, 0, SEEK_END) != 0)
    {
        SIMDATA_PRINT("write file", "seek to end failed");
        return 0;
    }

    file_size = ftell(data_obj->simdata_file);

   if (fseek(data_obj->simdata_file, 0, SEEK_SET) != 0)
    {
        SIMDATA_PRINT("write file", "seek to start failed");
        return 0;
    }

    /* close file */
    if (fclose(data_obj->simdata_file) != 0)
    {
        SIMDATA_PRINT("write file", "Close file failed");
        return 0;
    }

    return file_size;
}

static bool SimDataFile_Dump(SimDataFileObj_TypeDef *data_obj, Stream_TypeDef *stream)
{
    uint32_t file_size = SimDataFile_GetSize(data_obj);
    char name_buf[strlen(data_obj->simdata_path_str) + strlen(data_obj->file_name) + strlen(Folder_Split)];

    if ((data_obj != NULL) && \
        (data_obj->simdata_file != NULL) && \
        (stream != NULL) && \
        (stream->size == file_size) && \
        (stream->p_buf != NULL))
    {
        memset(name_buf, '\0', (strlen(data_obj->simdata_path_str) + strlen(data_obj->file_name)));
        strcpy(name_buf, data_obj->simdata_path_str);
        strcat(name_buf, Folder_Split);
        strcat(name_buf, data_obj->file_name);
        
        /* open file */
        if ((data_obj->simdata_file = fopen(name_buf, "rb+")) == NULL)
        {
            SIMDATA_PRINT("dump file", "Open file %s failed", name_buf);
            return false;
        }
        
        if (fseek(data_obj->simdata_file, 0, 0) != 0)
        {
            SIMDATA_PRINT("dump file", "seek to start failed");
            return false;
        }

        /* read file */
        if (fread(stream->p_buf, sizeof(uint8_t), file_size, data_obj->simdata_file) <= 0)
        {
            SIMDATA_PRINT("dump file", "failed");
            return false;
        }

        /* close file */
        if (fclose(data_obj->simdata_file) != 0)
        {
            SIMDATA_PRINT("dump file", "Close file failed");
            return false;
        }
    
        return true;
    }

    return false;
}
