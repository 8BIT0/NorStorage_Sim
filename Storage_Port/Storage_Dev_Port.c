#include "Storage_Bus_Port.h"
#include "Storage_Dev_Port.h"
#include "Storage_Def.h"

/* if you use this code on some hardware platform */
/* add you own headers on this file */

/* only when sim use this header */
#include "../Dep/Runtime.h"
#include "../SimDevModule/SimDataFile_Opr.h"

#define Storage_Dev_GetSstsTick SrvOsCommon.get_os_ms
#define Storage_Dev_Malloc(x)   malloc(x)
#define Storage_Dev_Free(x)     free(x)

/* debug */
#define STORAGE_DEV_TAG                     "SimDevice"
#define STORAGE_DEV_INFO(stage, fmt, ...)   Debug_Print(STORAGE_DEV_TAG, stage, fmt, ##__VA_ARGS__)

/* internal vriable */
static uint8_t read_tmp[Storage_TabSize * 2] __attribute__((aligned(4))) = {0};
static uint8_t write_tmp[Storage_TabSize * 2] __attribute__((aligned(4))) = {0};

/* external function */
static bool Storage_Dev_Set(StorageDevObj_TypeDef *ext_dev);
static bool Storage_Dev_Init(StorageDevObj_TypeDef *ext_dev, uint16_t *p_type, uint16_t *p_code);

static bool Storage_Dev_Param_Read(StorageDevObj_TypeDef *p_dev, uint32_t base_addr, uint32_t addr_offset, uint8_t *p_data, uint32_t len);
static bool Storage_Dev_Param_Write(StorageDevObj_TypeDef *p_dev, uint32_t base_addr, uint32_t addr_offset, uint8_t *p_data, uint32_t len);
static bool Storage_Dev_Param_Erase(StorageDevObj_TypeDef *p_dev, uint32_t base_addr, uint32_t addr_offset, uint32_t len);

StorageDevApi_TypeDef StorageDev = {
    .set = Storage_Dev_Set,
    .init = Storage_Dev_Init,

    .write_phy_sec = NULL,
    .read_phy_sec = NULL,
    .erase_phy_sec = NULL,

    .firmware_format = NULL,
    .firmware_read = NULL,
    .firmware_write = NULL,

    .param_read = Storage_Dev_Param_Read,
    .param_write = Storage_Dev_Param_Write,
    .param_erase = Storage_Dev_Param_Erase,
};

static bool Storage_Dev_Set(StorageDevObj_TypeDef *ext_dev)
{
    if ((ext_dev == NULL) || \
        (ext_dev->api == NULL))
        return false;

    ext_dev->start_addr  = 0;
    ext_dev->sector_num  = 0;
    ext_dev->sector_size = 0;
    ext_dev->total_size  = 0;
    ext_dev->page_num    = 0;
    ext_dev->page_size   = 0;

    ext_dev->obj = Storage_Dev_Malloc(sizeof(SimDevW25QxxObj_TypeDef));
    if (ext_dev->obj == NULL)
        return false;

    To_DevW25Qxx_OBJ(ext_dev->obj)->systick   = Runtime.get_ms;
    To_DevW25Qxx_OBJ(ext_dev->obj)->cs_ctl    = NULL;
    To_DevW25Qxx_OBJ(ext_dev->obj)->bus_tx    = StoragePort_Api.bus_tx;
    To_DevW25Qxx_OBJ(ext_dev->obj)->bus_rx    = StoragePort_Api.bus_rx;
    To_DevW25Qxx_OBJ(ext_dev->obj)->delay_ms  = NULL;
    return true;
}

static bool Storage_Dev_Init(StorageDevObj_TypeDef *ext_dev, uint16_t *p_type, uint16_t *p_code)
{
    uint8_t init_state = 0;

    if ((ext_dev != NULL) && \
        (ext_dev->chip_type >= Storage_ChipType_W25Q08) && \
        (ext_dev->chip_type <= Storage_ChipType_W25Q128))
    {
        if ((To_DevW25Qxx_API(ext_dev->api)->init == NULL) || \
            (To_DevW25Qxx_API(ext_dev->api)->info == NULL))
        {
            STORAGE_DEV_INFO("init", "API Invalid");
            return false;
        }

        /* need to set simulation device type */
        switch (ext_dev->chip_type)
        {
            case Storage_ChipType_W25Q08:To_DevW25Qxx_OBJ(ext_dev->obj)->type = SimDev_W25Q08;    break;
            case Storage_ChipType_W25Q16:To_DevW25Qxx_OBJ(ext_dev->obj)->type = SimDev_W25Q16;    break;
            case Storage_ChipType_W25Q32:To_DevW25Qxx_OBJ(ext_dev->obj)->type = SimDev_W25Q32;    break;
            case Storage_ChipType_W25Q64:To_DevW25Qxx_OBJ(ext_dev->obj)->type = SimDev_W25Q64;    break;
            case Storage_ChipType_W25Q128: To_DevW25Qxx_OBJ(ext_dev->obj)->type = SimDev_W25Q128; break;
            default: To_DevW25Qxx_OBJ(ext_dev->obj)->type = SimDev_W25Q128; break;
        }

        STORAGE_DEV_INFO("api", "init");
        init_state = To_DevW25Qxx_API(ext_dev->api)->init(To_DevW25Qxx_OBJ(ext_dev->obj));
        /* only when you use some hardware platform */
        // *p_type = To_DevW25Qxx_API(ext_dev->api)->info(To_DevW25Qxx_OBJ(ext_dev->obj)).prod_type;
        *p_code = To_DevW25Qxx_API(ext_dev->api)->info(To_DevW25Qxx_OBJ(ext_dev->obj)).prod_code;
    
        ext_dev->start_addr  = W25QXX_BASE_ADDRESS;
        ext_dev->sector_num  = To_DevW25Qxx_API(ext_dev->api)->info(To_DevW25Qxx_OBJ(ext_dev->obj)).sector_num;
        ext_dev->sector_size = To_DevW25Qxx_API(ext_dev->api)->info(To_DevW25Qxx_OBJ(ext_dev->obj)).sector_size;
        ext_dev->total_size  = To_DevW25Qxx_API(ext_dev->api)->info(To_DevW25Qxx_OBJ(ext_dev->obj)).flash_size;
        ext_dev->page_num    = To_DevW25Qxx_API(ext_dev->api)->info(To_DevW25Qxx_OBJ(ext_dev->obj)).page_num;
        ext_dev->page_size   = To_DevW25Qxx_API(ext_dev->api)->info(To_DevW25Qxx_OBJ(ext_dev->obj)).page_size;

        STORAGE_DEV_INFO("api", "Init %s", ((SimDevW25Qxx_Error_List)init_state == SimDevW25Qxx_Ok) ? "done" : "failed");
        return ((SimDevW25Qxx_Error_List)init_state == SimDevW25Qxx_Ok) ? true : false;
    }

    STORAGE_DEV_INFO("init", "Invalid parameter");
    return false;
}

static bool Storage_Dev_Param_Read(StorageDevObj_TypeDef *p_dev, uint32_t base_addr, uint32_t addr_offset, uint8_t *p_data, uint32_t len)
{
    uint32_t read_start_addr = base_addr + addr_offset;
    uint32_t flash_end_addr = 0;
    uint32_t section_start_addr = 0;
    uint32_t next_read_addr = 0;
    uint32_t section_size = 0;
    uint32_t read_offset = 0;
    uint32_t read_len = len;

    if ((p_dev == NULL) || \
        (p_dev->api == NULL) || \
        (p_dev->obj == NULL) || \
        (sizeof(read_tmp) < len) || \
        (p_data == 0) || \
        (len == 0))
        return false;

    section_size = To_DevW25Qxx_API(p_dev->api)->info(To_DevW25Qxx_OBJ(p_dev->obj)).sector_size;
    /* get w25qxx device info */
    /* address check */
    flash_end_addr = To_DevW25Qxx_API(p_dev->api)->info(To_DevW25Qxx_OBJ(p_dev->obj)).start_addr;
    if (flash_end_addr > read_start_addr)
        return false;

    /* range check */
    flash_end_addr += To_DevW25Qxx_API(p_dev->api)->info(To_DevW25Qxx_OBJ(p_dev->obj)).flash_size;
    if ((len + read_start_addr) > flash_end_addr)
        return false;

    if (section_size == 0)
        return false;
        
    section_start_addr = To_DevW25Qxx_API(p_dev->api)->get_section_start_addr(To_DevW25Qxx_OBJ(p_dev->obj), read_start_addr);
    read_offset = read_start_addr - section_start_addr;
    if (section_size > sizeof(read_tmp))
        return false;

    while(true)
    {
        /* circumstances 1: store data size not bigger than flash sector size and only none multiple sector write is needed */
        /* circumstances 2: store data size not bigger than flash sector length but need to write from the end of the sector N to the start of the sector N + 1 */
        /* circumstances 3: store data size large than flash sector length multiple sector write is needed */
        if (read_offset + read_len > section_size)
            read_len = section_size - read_offset;

        /* read whole section */
        if (To_DevW25Qxx_API(p_dev->api)->read_sector(To_DevW25Qxx_OBJ(p_dev->obj), section_start_addr, read_tmp, section_size) != SimDevW25Qxx_Ok)
            return false;
    
        memcpy(p_data, read_tmp + read_offset, read_len);
        memset(read_tmp, 0, section_size);

        len -= read_len;
        if (len == 0)
            return true;
    
        read_offset = 0;
        next_read_addr = To_DevW25Qxx_API(p_dev->api)->get_section_start_addr(To_DevW25Qxx_OBJ(p_dev->obj), section_start_addr + read_len);
        if (next_read_addr == section_start_addr)
            read_offset = read_len;
        
        p_data += read_len;
        read_len = len;
        section_start_addr = next_read_addr;
    }
}

static bool Storage_Dev_Param_Write(StorageDevObj_TypeDef *p_dev, uint32_t base_addr, uint32_t addr_offset, uint8_t *p_data, uint32_t len)
{
    uint32_t write_start_addr = base_addr + addr_offset;
    uint32_t flash_end_addr = 0;
    uint32_t section_start_addr = 0;
    uint32_t next_write_addr = 0;
    uint32_t section_size = 0;
    uint32_t write_offset = 0;
    uint32_t write_len = len;

    if ((p_dev == NULL) || \
        (p_dev->api == NULL) || \
        (p_dev->obj == NULL) || \
        (p_data == NULL) || \
        (len == 0))
        return false;

    section_size = To_DevW25Qxx_API(p_dev->api)->info(To_DevW25Qxx_OBJ(p_dev->obj)).sector_size;
    /* get w25qxx device info */
    /* address check */
    flash_end_addr = To_DevW25Qxx_API(p_dev->api)->info(To_DevW25Qxx_OBJ(p_dev->obj)).start_addr;
    if (flash_end_addr > write_start_addr)
        return false;

    /* range check */
    flash_end_addr += To_DevW25Qxx_API(p_dev->api)->info(To_DevW25Qxx_OBJ(p_dev->obj)).flash_size;
    if ((len + write_start_addr) > flash_end_addr)
        return false;
    
    if (section_size == 0)
        return false;
        
    section_start_addr = To_DevW25Qxx_API(p_dev->api)->get_section_start_addr(To_DevW25Qxx_OBJ(p_dev->obj), write_start_addr);
    write_offset = write_start_addr - section_start_addr;
    if (section_size > sizeof(write_tmp))
        return false;

    while(true)
    {
        /* circumstances 1: store data size not bigger than flash sector size and only none multiple sector write is needed */
        /* circumstances 2: store data size not bigger than flash sector length but need to write from the end of the sector N to the start of the sector N + 1 */
        /* circumstances 3: store data size large than flash sector length multiple sector write is needed */
        /* read whole section */
        if (To_DevW25Qxx_API(p_dev->api)->read_sector(To_DevW25Qxx_OBJ(p_dev->obj), section_start_addr, write_tmp, section_size) != SimDevW25Qxx_Ok)
            return false;

        /* erase whole section */
        if (To_DevW25Qxx_API(p_dev->api)->erase_sector(To_DevW25Qxx_OBJ(p_dev->obj), section_start_addr) != SimDevW25Qxx_Ok)
            return false;

        /* update whole section */
        if (write_offset + write_len > section_size)
            write_len = section_size - write_offset;
        
        /* copy data to section data read out */
        memcpy(write_tmp + write_offset, p_data, write_len);

        SimDevW25Qxx_Error_List state = To_DevW25Qxx_API(p_dev->api)->write_sector(To_DevW25Qxx_OBJ(p_dev->obj), section_start_addr, write_tmp, section_size);

        /* clear cache buff */
        memset(write_tmp, 0, section_size);
        
        len -= write_len;
        if (state == SimDevW25Qxx_Ok)
        {
            if (len == 0)
                return true;
        }
        else
            return false;

        write_offset = 0;
        next_write_addr = To_DevW25Qxx_API(p_dev->api)->get_section_start_addr(To_DevW25Qxx_OBJ(p_dev->obj), section_start_addr + write_len);
        if (next_write_addr == section_start_addr)
            write_offset = write_len;

        p_data += write_len;
        write_len = len;
        section_start_addr = next_write_addr; 
    }

    return false;
}

static bool Storage_Dev_Param_Erase(StorageDevObj_TypeDef *p_dev, uint32_t base_addr, uint32_t addr_offset, uint32_t len)
{
    uint32_t erase_start_addr = base_addr + addr_offset;

    if ((p_dev == NULL) || \
        (p_dev->api == NULL) || \
        (p_dev->obj == NULL) || \
        (len == 0))
        return false;

    /* get w25qxx device info */
    /* address check */
    if (erase_start_addr < To_DevW25Qxx_API(p_dev->api)->info(To_DevW25Qxx_OBJ(p_dev->obj)).start_addr)
        return false;

    /* W25Qxx device read */
    if (To_DevW25Qxx_API(p_dev->api)->erase_sector(To_DevW25Qxx_OBJ(p_dev->obj), erase_start_addr) == SimDevW25Qxx_Ok)
        return true;
    
    return false;
}
