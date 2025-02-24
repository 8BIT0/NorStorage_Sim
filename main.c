#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include "SimDevModule/SimDataFile_Opr.h"
#include "Storage_Port/Storage.h"
#include "py_dsp_tool.h"
#include "SysCommon.h"
#include "PY_Tool/Callback_Dep/ui_callback.h"

#define SIMULATION_TAG "SIM"
#define SIMULATION_PRINT(stage, fmt, ...) Debug_Print(SIMULATION_TAG, stage, fmt, ##__VA_ARGS__) 

/* internal vriable */
static SimDataFileObj_TypeDef SimObj;
static StorageDevObj_TypeDef SimDev;
static Thread_Hdl SimPolling_hdl;

/* internal function */
static bool SimModule_Init(char *app_path);
static void* Sim_Polling_Thread(void *arg);
static bool Sim_Link_Lib(char *exe_path);

static bool Sim_Storage_Create_Callback(TriggerData_TypeDef *data);
static bool Sim_Storage_Modify_Callback(TriggerData_TypeDef *data);
static bool Sim_Storage_Delete_Callback(TriggerData_TypeDef *data);
static bool Sim_Storage_Search_Callback(TriggerData_TypeDef *data);

int main(int argc, char **argv)
{
    if (!SimModule_Init(argv[0]))
    {
        /* module init filed */
        SIMULATION_PRINT("Storage", "init failed");
        return 0;
    }

    /* create simulation polling thread */
    SimPolling_hdl = Sys_CreateThread((Thread_Callback)Sim_Polling_Thread);
    if (SimPolling_hdl == NULL)
    {
        SIMULATION_PRINT("thread", "Polling thread create failed");
        return 0;
    }
    else
        SIMULATION_PRINT("thread", "Polling thread create done");

    /* storage module init successed */
    /* init python tool -> visualize module */
    if (!PY_Visualize.init(SimObj.simdata_path_str, SimObj.file_name, Flash_Start_Addr))
    {
        SIMULATION_PRINT("Visualize", "init failed");
        return 0;
    }

    if (!Sim_Link_Lib(argv[0]))
        return 0;

    PY_Visualize.update();

    Sys_CancelThread(SimPolling_hdl);
    SIMULATION_PRINT("thread", "Qiut");

    return 0;
}

static bool Sim_Link_Lib(char *exe_path)
{
    void *StorageModule_Lib = NULL;
    void (*set_ui_callback)(TriggerType_List, trigger_callback);
    char *lib_path = NULL;
    char *app_path = NULL;
    uint16_t path_len = 1;

    /* link StorageModule.so */
    path_len += strlen(exe_path);
    path_len += strlen("PY_Tool");
    path_len += strlen("Callback_Dep");
    path_len += strlen("build");
    path_len += strlen("StorageModule");
    path_len += strlen(Lib_Extend);
    path_len += strlen(Folder_Split) * 4;
    lib_path = Sys_Malloc(path_len);
    if (lib_path == NULL)
    {
        SIMULATION_PRINT("lib path", "Malloc failed");
        return false;
    }

    app_path = dirname(exe_path);
    char *lst_spl = strrchr(app_path, Folder_Split[0]);

    if (!lst_spl)
    {
        SIMULATION_PRINT("lib", "Path error");
        return false;
    }

    *lst_spl = '\0';
    strcpy(lib_path, app_path);
    strcat(lib_path, Folder_Split);
    strcat(lib_path, "PY_Tool");
    strcat(lib_path, Folder_Split);
    strcat(lib_path, "Callback_Dep");
    strcat(lib_path, Folder_Split);
    strcat(lib_path, "build");
    strcat(lib_path, Folder_Split);
    strcat(lib_path, "StorageModule");
    strcat(lib_path, Lib_Extend);
    SIMULATION_PRINT("Lib path", lib_path);

    StorageModule_Lib = dlopen(lib_path, RTLD_LAZY);
    if (StorageModule_Lib == NULL)
    {
        SIMULATION_PRINT("Lib", "Link failed", dlerror());
        Sys_Free((void **)&lib_path);
        return false;
    }
    SIMULATION_PRINT("Lib", "Link successed");
    dlerror();

    /* set callback */
    set_ui_callback = (void (*)(TriggerType_List, trigger_callback))dlsym(StorageModule_Lib, "UICallback_Set");
    if (dlerror() != NULL)
    {
        SIMULATION_PRINT("Lib", "Call mathod failed", dlerror());
        Sys_Free((void **)&lib_path);
        return false;
    }

    set_ui_callback(Trigger_Create, Sim_Storage_Create_Callback);
    set_ui_callback(Trigger_Modify, Sim_Storage_Modify_Callback);
    set_ui_callback(Trigger_Delete, Sim_Storage_Delete_Callback);
    set_ui_callback(Trigger_Search, Sim_Storage_Search_Callback);
    
    return true;
}

static bool SimModule_Init(char *app_path)
{
    int type = -1;
    const char* folder_path = NULL;
    const char* sim_name = NULL;
    uint32_t file_size = 0;
    bool module_init = false;

    memset(&SimDev, 0, sizeof(StorageDevObj_TypeDef));
    
    SimObj.malloc = Sys_Malloc;
    SimObj.free = Sys_Free;

    SimDev.chip_type = Storage_ChipType_W25Q128;
    SimDev.api = (void *)(&SimDevW25Qxx);
    SimDev.dev_obj = (void *)(&SimObj);

    /* wait input sim type */
    while (true)
    {
        SIMULATION_PRINT("select", "simulator confirm");
        SIMULATION_PRINT("option", "%d ---- W25Q08",  Storage_ChipType_W25Q08);
        SIMULATION_PRINT("option", "%d ---- W25Q16",  Storage_ChipType_W25Q16);
        SIMULATION_PRINT("option", "%d ---- W25Q32",  Storage_ChipType_W25Q32);
        SIMULATION_PRINT("option", "%d ---- W25Q64",  Storage_ChipType_W25Q64);
        SIMULATION_PRINT("option", "%d ---- W25Q128", Storage_ChipType_W25Q128);
        while (true)
        {
            if ((scanf("%d", &type) == 0) || \
                (type < Storage_ChipType_W25Q08) || \
                (type > Storage_ChipType_W25Q128))
            {
                SIMULATION_PRINT("selection", "unknow type input %d", type);
                rewind(stdin);
                continue;
            }

            /* check input type */
            switch (type)
            {
                case Storage_ChipType_W25Q08:
                    SIMULATION_PRINT("selection", "W25Q08");
                    sim_name = "W25Q08";
                    file_size = Dev_W25Q08_MSize;
                    break;

                case Storage_ChipType_W25Q16:
                    SIMULATION_PRINT("selection", "W25Q16");
                    sim_name = "W25Q16";
                    file_size = Dev_W25Q16_MSize;
                    break;
                
                case Storage_ChipType_W25Q32:
                    SIMULATION_PRINT("selection", "W25Q32");
                    sim_name = "W25Q32";
                    file_size = Dev_W25Q32_MSize;
                    break;

                case Storage_ChipType_W25Q64:
                    SIMULATION_PRINT("selection", "W25Q64");
                    sim_name = "W25Q64";
                    file_size = Dev_W25Q64_MSize;
                    break;
                
                case Storage_ChipType_W25Q128:
                    SIMULATION_PRINT("selection", "W25Q128");
                    sim_name = "W25Q128";
                    file_size = Dev_W25Q128_MSize;
                    break;
            }
            break;

            Sleep_Ms(10);
        }

        if (SimDataFile.create(&SimObj, app_path, sim_name, file_size))
        {
            /* storage module init */
            module_init = Storage.init(&SimDev);
            
            SIMULATION_PRINT("module init", "state %s", module_init ? "true" : "false");
            return module_init;
        }
        
        break;
    }

    return false;
}

/* wait python widget input */
/* thread frequence 100Hz */
static void* Sim_Polling_Thread(void *arg)
{
    while (true)
    {
        Sleep_Ms(10);
    }
}

/* push create item info into queue */
static bool Sim_Storage_Create_Callback(TriggerData_TypeDef *data)
{
    Storage_ParaClassType_List cls = Para_Sys;

    if (data == NULL)
        return false;

    if (data->sec == UserSec)
        cls = Para_User;

    if (Storage.create(cls, data->name, data->data, data->size) != Storage_Error_None)
    {
        SIMULATION_PRINT("create item", "%s failed", data->name);
        return false;
    }

    SIMULATION_PRINT("create item", "%s succcessed", data->name);
    return true;
}

static bool Sim_Storage_Modify_Callback(TriggerData_TypeDef *data)
{
    if (data == NULL)
        return false;

    return true;
}

static bool Sim_Storage_Search_Callback(TriggerData_TypeDef *data)
{
    if (data == NULL)
        return false;

    return true;
}

static bool Sim_Storage_Delete_Callback(TriggerData_TypeDef *data)
{
    if (data == NULL)
        return false;

    return true;
}






