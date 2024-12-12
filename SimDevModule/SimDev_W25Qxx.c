#include "SimDev_W25Qxx.h"

#define W25QXX_TIMEOUT  100  /* unit:ms */
#define DEFAULT_DATA    0xFF

/* external function */
static SimDevW25Qxx_Error_List SimDevW25Qxx_Init(SimDevW25QxxObj_TypeDef *dev);
static SimDev_Info_TypeDef SimDevW25Qxx_GetInfo(SimDevW25QxxObj_TypeDef *dev);
static int32_t SimDevW25Qxx_Get_SectionByDataAddress(SimDevW25QxxObj_TypeDef *dev, uint32_t addr);
static SimDevW25Qxx_Error_List SimDevW25Qxx_WriteSector(SimDevW25QxxObj_TypeDef *dev, uint32_t addr, uint8_t *tx, uint32_t size);
static SimDevW25Qxx_Error_List SimDevW25Qxx_ReadSector(SimDevW25QxxObj_TypeDef *dev, uint32_t addr, uint8_t *rx, uint32_t size);
static SimDevW25Qxx_Error_List SimDevW25Qxx_EraseSector(SimDevW25QxxObj_TypeDef *dev, uint32_t addr);
static SimDevW25Qxx_Error_List SimDevW25Qxx_EraseChip(SimDevW25QxxObj_TypeDef *dev);

SimDevW25Qxx_TypeDef SimDevW25Qxx = {
    .init = SimDevW25Qxx_Init,
    .info = SimDevW25Qxx_GetInfo,
    .get_section_start_addr = SimDevW25Qxx_Get_SectionByDataAddress,
    .write_sector = SimDevW25Qxx_WriteSector,
    .read_sector = SimDevW25Qxx_ReadSector,
    .erase_sector = SimDevW25Qxx_EraseSector,
    .erase_chip = SimDevW25Qxx_EraseChip,
};

static SimDevW25Qxx_Error_List SimDevW25Qxx_Init(SimDevW25QxxObj_TypeDef *dev)
{
    if ((dev == NULL) || \
        (dev->systick == NULL) || \
        (dev->type <= SimDev_None) || \
        (dev->type >= SimDev_All))
        return SimDevW25Qxx_Error;

    dev->start_time = 0;
    dev->info.prod_type = dev->type;

    /* set device info */
    switch ((uint8_t)dev->type)
    {
        case SimDev_W25Q08:
            dev->info.name        = W25Q08_NAME_STR;
            dev->info.block_num   = W25Q08FV_BLOCK_NUM;
            dev->info.block_size  = W25Q08FV_BLOCK_SIZE;
            dev->info.page_num    = W25Q08FV_PAGE_NUM;
            dev->info.page_size   = W25Q08FV_PAGE_SIZE;
            dev->info.sector_num  = W25Q08FV_SECTOR_NUM;
            dev->info.sector_size = W25Q08FV_SECTOR_SIZE;
            dev->info.flash_size  = W25Q08FV_FLASH_SIZE;
            break;
    
        case SimDev_W25Q16:
            dev->info.name        = W25Q16_NAME_STR;
            dev->info.block_num   = W25Q16FV_BLOCK_NUM;
            dev->info.block_size  = W25Q16FV_BLOCK_SIZE;
            dev->info.page_num    = W25Q16FV_PAGE_NUM;
            dev->info.page_size   = W25Q16FV_PAGE_SIZE;
            dev->info.sector_num  = W25Q16FV_SECTOR_NUM;
            dev->info.sector_size = W25Q16FV_SECTOR_SIZE;
            dev->info.flash_size  = W25Q16FV_FLASH_SIZE;
            break;

        case SimDev_W25Q32:
            dev->info.name        = W25Q32_NAME_STR;
            dev->info.block_num   = W25Q32FV_BLOCK_NUM;
            dev->info.block_size  = W25Q32FV_BLOCK_SIZE;
            dev->info.page_num    = W25Q32FV_PAGE_NUM;
            dev->info.page_size   = W25Q32FV_PAGE_SIZE;
            dev->info.sector_num  = W25Q32FV_SECTOR_NUM;
            dev->info.sector_size = W25Q32FV_SECTOR_SIZE;
            dev->info.flash_size  = W25Q32FV_FLASH_SIZE;
            break;

        case SimDev_W25Q64:
            dev->info.name        = W25Q64_NAME_STR;
            dev->info.block_num   = W25Q64FV_BLOCK_NUM;
            dev->info.block_size  = W25Q64FV_BLOCK_SIZE;
            dev->info.page_num    = W25Q64FV_PAGE_NUM;
            dev->info.page_size   = W25Q64FV_PAGE_SIZE;
            dev->info.sector_num  = W25Q64FV_SECTOR_NUM;
            dev->info.sector_size = W25Q64FV_SECTOR_SIZE;
            dev->info.flash_size  = W25Q64FV_FLASH_SIZE;
            break;

        case SimDev_W25Q128:
            dev->info.name        = W25Q128_NAME_STR;
            dev->info.block_num   = W25Q128FV_BLOCK_NUM;
            dev->info.block_size  = W25Q128FV_BLOCK_SIZE;
            dev->info.page_num    = W25Q128FV_PAGE_NUM;
            dev->info.page_size   = W25Q128FV_PAGE_SIZE;
            dev->info.sector_num  = W25Q128FV_SECTOR_NUM;
            dev->info.sector_size = W25Q128FV_SECTOR_SIZE;
            dev->info.flash_size  = W25Q128FV_FLASH_SIZE;
            break;

        default: return SimDevW25Qxx_Error;
    }

    dev->start_time = dev->systick();
    return SimDevW25Qxx_Ok;
}

static SimDev_Info_TypeDef SimDevW25Qxx_GetInfo(SimDevW25QxxObj_TypeDef *dev)
{
    SimDev_Info_TypeDef info_tmp;

    memset(&info_tmp, 0, sizeof(SimDev_Info_TypeDef));

    if (dev && dev->start_time)
        info_tmp = dev->info;

    return info_tmp;
}

static int32_t SimDevW25Qxx_Get_SectionByDataAddress(SimDevW25QxxObj_TypeDef *dev, uint32_t addr)
{
    if ((dev == NULL) || (dev->start_time == 0))
        return -1;

    return (addr / dev->info.sector_size);
}

/* only support write or read whole sector with whole size from the first byte */
static SimDevW25Qxx_Error_List SimDevW25Qxx_WriteSector(SimDevW25QxxObj_TypeDef *dev, uint32_t addr, uint8_t *tx, uint32_t size)
{
    uint16_t ret = 0;

    if ((dev == NULL) || \
        (dev->start_time == 0) || \
        (dev->bus_write == NULL) || \
        (addr % dev->info.sector_size) || \
        (tx == NULL) || (dev->info.sector_size != size))
        return SimDevW25Qxx_Error;

    ret = dev->bus_write(addr, tx, size, W25QXX_TIMEOUT);
    if (ret == 0)
        return SimDevW25Qxx_Error;

    return SimDevW25Qxx_Ok;
}

static SimDevW25Qxx_Error_List SimDevW25Qxx_ReadSector(SimDevW25QxxObj_TypeDef *dev, uint32_t addr, uint8_t *rx, uint32_t size)
{
    uint16_t ret = 0;

    if ((dev == NULL) || \
        (dev->start_time == 0) || \
        (dev->bus_read == NULL) || \
        (addr % dev->info.sector_size) || \
        (rx == NULL) || (dev->info.sector_size != size))
        return SimDevW25Qxx_Error;
    
    ret = dev->bus_read(addr, rx, size, W25QXX_TIMEOUT);
    if (ret == 0)
        return SimDevW25Qxx_Error;

    return SimDevW25Qxx_Ok;
}

static SimDevW25Qxx_Error_List SimDevW25Qxx_EraseSector(SimDevW25QxxObj_TypeDef *dev, uint32_t addr)
{
    if ((dev == NULL) || \
        (dev->start_time == 0) || \
        (dev->bus_write == NULL) || \
        (addr % dev->info.sector_size))
        return SimDevW25Qxx_Error;
    
    uint8_t tmp_buf[dev->info.sector_size];
    memset(tmp_buf, DEFAULT_DATA, dev->info.sector_size);

    /* write 0xFF to target sector */
    return SimDevW25Qxx_WriteSector(dev, addr, tmp_buf, dev->info.sector_size);
}

static SimDevW25Qxx_Error_List SimDevW25Qxx_EraseChip(SimDevW25QxxObj_TypeDef *dev)
{
    if ((dev == NULL) || \
        (dev->start_time == 0))
        return SimDevW25Qxx_Error;

    for (uint16_t i = 0; i < dev->info.sector_size; i ++)
    {
        if (SimDevW25Qxx_EraseSector(dev, (i * dev->info.sector_size)) != SimDevW25Qxx_Ok)
            return SimDevW25Qxx_Error;
    }

    return SimDevW25Qxx_Ok;
}
