#ifndef __STORAGE_DEV_PORT_H
#define __STORAGE_DEV_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "../Dep/util.h"
#include "SimDev_W25Qxx.h"

#define To_StorageDevObj_Ptr(x) ((StorageDevObj_TypeDef *)x)

#define Dev_W25Q08_MSize    W25Q08FV_FLASH_SIZE
#define Dev_W25Q16_MSize    W25Q16FV_FLASH_SIZE
#define Dev_W25Q32_MSize    W25Q32FV_FLASH_SIZE
#define Dev_W25Q64_MSize    W25Q64FV_FLASH_SIZE
#define Dev_W25Q128_MSize   W25Q128FV_FLASH_SIZE

typedef enum
{
    Storage_Chip_None = 0,
    Storage_ChipType_W25Q08,
    Storage_ChipType_W25Q16,
    Storage_ChipType_W25Q32,
    Storage_ChipType_W25Q64,
    Storage_ChipType_W25Q128,
    Storage_ChipType_All,
} Storage_ExtFlashChipType_List;

/* hadware flash chip info */
typedef struct
{
    Storage_ExtFlashChipType_List chip_type;
    
    uint32_t start_addr;
    uint32_t total_size;

    uint32_t page_num;
    uint32_t page_size;

    uint32_t bank_num;
    uint32_t bank_size;

    uint32_t block_num;
    uint32_t block_size;

    uint32_t sector_num;
    uint32_t sector_size;

    uint32_t bad_block_num;

    void *obj;
    void *api;
} StorageDevObj_TypeDef;

typedef struct
{
    bool (*set)(StorageDevObj_TypeDef *ext_dev);
    bool (*init)(StorageDevObj_TypeDef *ext_dev, uint16_t *p_type, uint16_t *p_code);

    /* directly write data to physical section */
    bool (*write_phy_sec)(StorageDevObj_TypeDef *p_dev, uint32_t addr, uint8_t *p_data, uint16_t len);
    bool (*read_phy_sec)(StorageDevObj_TypeDef *p_dev, uint32_t addr, uint8_t *p_data, uint16_t len);
    bool (*erase_phy_sec)(StorageDevObj_TypeDef *p_dev, uint32_t addr, uint16_t len);

    bool (*firmware_format)(StorageDevObj_TypeDef *p_dev, uint32_t addr, uint32_t firmware_size);
    bool (*firmware_read)(StorageDevObj_TypeDef *p_dev, uint32_t base_addr, uint32_t addr_offset, uint8_t *p_data, uint16_t size);
    bool (*firmware_write)(StorageDevObj_TypeDef *p_dev, uint32_t base_addr, uint32_t addr_offset, uint8_t *p_data, uint16_t size);

    bool (*param_write)(StorageDevObj_TypeDef *p_dev, uint32_t base_addr, uint32_t addr_offset, uint8_t *p_data, uint32_t len);
    bool (*param_read)(StorageDevObj_TypeDef *p_dev, uint32_t base_addr, uint32_t addr_offset, uint8_t *p_data, uint32_t len);
    bool (*param_erase)(StorageDevObj_TypeDef *p_dev, uint32_t base_addr, uint32_t addr_offset, uint32_t len);
} StorageDevApi_TypeDef;

extern StorageDevApi_TypeDef StorageDev;

#ifdef __cplusplus
}
#endif

#endif
