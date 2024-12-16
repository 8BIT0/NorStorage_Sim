#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "SimDevModule/SimDataFile_Opr.h"
#include "Storage_Port/Storage.h"
#include "Dep/Runtime.h"

#define SIMULATION_TAG "Simulation"
#define SIMULATION_PRINT(stage, fmt, ...) Debug_Print(SIMULATION_TAG, stage, fmt, ##__VA_ARGS__) 

/* internal vriable */
static SimDataFileObj_TypeDef SimObj;

/* internal function */
static bool SimModule_Init(char *app_path);
static void* Sim_Malloc(uint32_t size);
static void Sim_Free(void *ptr);

int main(int argc, char **argv)
{
    SimModule_Init(argv[0]);

    /* storage module init */

    /* main logic run */
    while (true)
    {
        
    }

    return 0;
}

static bool SimModule_Init(char *app_path)
{
    int type = -1;
    const char* folder_path = NULL;
    const char* sim_name = NULL;
    uint32_t file_size = 0;

    SimObj.malloc = Sim_Malloc;
    SimObj.free = Sim_Free;
    
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
            scanf("%d", &type);
            /* check input type */
            if ((type >= Storage_ChipType_W25Q08) && \
                (type <= Storage_ChipType_W25Q128))
            {
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
            }
            else
            {
                rewind(stdin);
                SIMULATION_PRINT("selection", "unknow type input %d", type);
                type = -1;
            }

            sleep(10);
        }

        return SimDataFile.create(&SimObj, app_path, sim_name, file_size);
    }

    return false;
}

static void* Sim_Malloc(uint32_t size)
{
    return malloc((size_t)size);
}

static void Sim_Free(void *ptr)
{
    free(ptr);
}

