/*  Author: 8_B!T0
 * 
 *  I modify this file as simulation needed
 *  you can modify it as your system needed
 *  such as you can set this file as your spi bus or qspi bus port source file
 */
#include "Storage_Bus_Port.h"
#include "SimDataFile_Opr.h"

/* external function */
static void *Storage_External_Chip_Bus_Init(StorageBus_Malloc_Callback p_malloc, StorageBus_Free_Callback p_free);
static uint16_t Storage_External_Chip_BusTx(uint32_t addr, uint8_t *p_data, uint16_t len, uint32_t time_out);
static uint16_t Storage_External_Chip_BusRx(uint32_t addr, uint8_t *p_data, uint16_t len, uint32_t time_out);

StorageBusApi_TypeDef StoragePort_Api = {
    .init = Storage_External_Chip_Bus_Init,
    .cs_ctl = NULL,
    .bus_tx = Storage_External_Chip_BusTx,
    .bus_rx = Storage_External_Chip_BusRx,
    .bus_trans = NULL,
};

/************************************************** External Flash IO API Section ************************************************/
/* create sim file opreate bus object */
static void* Storage_External_Chip_Bus_Init(StorageBus_Malloc_Callback p_malloc, StorageBus_Free_Callback p_free)
{
    SimDataFileObj_TypeDef *file_obj = NULL;

    if ((p_malloc == NULL) || (p_free == NULL))
        return NULL;

    file_obj = p_malloc(sizeof(SimDataFileObj_TypeDef));
    if (file_obj == NULL)
    {
        p_free(file_obj);
        return NULL;
    }

    memset(file_obj, 0, sizeof(SimDataFile_TypeDef));
    file_obj->malloc = p_malloc;
    file_obj->free = p_free;

    return file_obj;
}

static uint16_t Storage_External_Chip_BusTx(uint32_t addr, uint8_t *p_data, uint16_t len, uint32_t time_out)
{
    if (p_data && len)
    {
        SimDataFile.write();
    }

    return 0;
}

static uint16_t Storage_External_Chip_BusRx(uint32_t addr, uint8_t *p_data, uint16_t len, uint32_t time_out)
{
    if (p_data && len)
    {
        SimDataFile.read();
    }

    return 0;
}
