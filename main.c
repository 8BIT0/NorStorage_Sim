#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "SimDevModule/SimDataFile_Opr.h"
#include "Storage_Port/Storage.h"
#include "py_dsp_tool.h"
#include "SysCommon.h"

#define SIMULATION_TAG "SIM"
#define SIMULATION_PRINT(stage, fmt, ...) Debug_Print(SIMULATION_TAG, stage, fmt, ##__VA_ARGS__) 

/* internal vriable */
static SimDataFileObj_TypeDef SimObj;
static StorageDevObj_TypeDef SimDev;
static Thread_Hdl SimPolling_hdl;

/* internal function */
static bool SimModule_Init(char *app_path);
static void* Sim_Polling_Thread(void *arg);

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

    Sys_CancelThread(SimPolling_hdl);
    SIMULATION_PRINT("thread", "Qiut");

    return 0;
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


