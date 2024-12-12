#ifndef __SIMDEV_W25QXX_H
#define __SIMDEV_W25QXX_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define To_DevW25Qxx_API(x)     ((SimDevW25Qxx_TypeDef *)x)
#define To_DevW25Qxx_OBJ(x)     ((SimDevW25QxxObj_TypeDef *)x)

#define W25QXX_PAGE_SIZE        0x100
#define W25QXX_SECTOR_SIZE      0x1000
#define W25QXX_BLOCK_SIZE       0x10000

#define W25Q08_NAME_STR         "W25Q08"
#define W25Q08FV_BLOCK_SIZE     W25QXX_BLOCK_SIZE   /* 16 block of 64KBytes */
#define W25Q08FV_SECTOR_SIZE    W25QXX_SECTOR_SIZE  /* 256 sectors of 4kBytes */
#define W25Q08FV_PAGE_SIZE      W25QXX_PAGE_SIZE    /* 4096 pages of 256 bytes */
#define W25Q08FV_BLOCK_NUM      16
#define W25Q08FV_SECTOR_NUM     256
#define W25Q08FV_PAGE_NUM       4096
                                                    /* 8 MBits => 1MBytes */
#define W25Q08FV_FLASH_SIZE     (W25Q08FV_BLOCK_NUM * W25QXX_BLOCK_SIZE)

#define W25Q16_NAME_STR         "W25Q16"
#define W25Q16FV_BLOCK_SIZE     W25QXX_BLOCK_SIZE   /* 32 block of 64KBytes */
#define W25Q16FV_SECTOR_SIZE    W25QXX_SECTOR_SIZE  /* 512 sectors of 4kBytes */
#define W25Q16FV_PAGE_SIZE      W25QXX_PAGE_SIZE    /* 8192 pages of 256 bytes */
#define W25Q16FV_BLOCK_NUM      32
#define W25Q16FV_SECTOR_NUM     512
#define W25Q16FV_PAGE_NUM       8192
                                                    /* 16 MBits => 2MBytes */
#define W25Q16FV_FLASH_SIZE     W25Q16FV_BLOCK_NUM * W25QXX_BLOCK_SIZE

#define W25Q32_NAME_STR         "W25Q32"
#define W25Q32FV_BLOCK_SIZE     W25QXX_BLOCK_SIZE   /* 64 block of 64KBytes */
#define W25Q32FV_SECTOR_SIZE    W25QXX_SECTOR_SIZE  /* 1024 sectors of 4kBytes */
#define W25Q32FV_PAGE_SIZE      W25QXX_PAGE_SIZE    /* 16384 pages of 256 bytes */
#define W25Q32FV_BLOCK_NUM      64
#define W25Q32FV_SECTOR_NUM     1024
#define W25Q32FV_PAGE_NUM       16384
                                                    /* 32 MBits => 4MBytes */
#define W25Q32FV_FLASH_SIZE     W25Q32FV_BLOCK_NUM * W25QXX_BLOCK_SIZE

#define W25Q64_NAME_STR         "W25Q64"
#define W25Q64FV_BLOCK_SIZE     W25QXX_BLOCK_SIZE   /* 128 block of 64KBytes */
#define W25Q64FV_SECTOR_SIZE    W25QXX_SECTOR_SIZE  /* 2048 sectors of 4kBytes */
#define W25Q64FV_PAGE_SIZE      W25QXX_PAGE_SIZE    /* 32767 pages of 256 bytes */
#define W25Q64FV_BLOCK_NUM      128
#define W25Q64FV_SECTOR_NUM     2048
#define W25Q64FV_PAGE_NUM       32768
                                                    /* 64 MBits => 8MBytes */
#define W25Q64FV_FLASH_SIZE     W25Q64FV_BLOCK_NUM * W25QXX_BLOCK_SIZE

#define W25Q128_NAME_STR        "W25Q128"
#define W25Q128FV_BLOCK_SIZE    W25QXX_BLOCK_SIZE   /* 256 block of 64KBytes */
#define W25Q128FV_SECTOR_SIZE   W25QXX_SECTOR_SIZE  /* 4096 sectors of 4kBytes */
#define W25Q128FV_PAGE_SIZE     W25QXX_PAGE_SIZE    /* 65536 pages of 256 bytes */
#define W25Q128FV_BLOCK_NUM     256
#define W25Q128FV_SECTOR_NUM    4096
#define W25Q128FV_PAGE_NUM      65535
                                                    /* 128 MBits => 16MBytes */
#define W25Q128FV_FLASH_SIZE    W25Q128FV_BLOCK_NUM * W25QXX_BLOCK_SIZE

typedef enum
{
    SimDev_None = -1,
    SimDev_W25Q08,
    SimDev_W25Q16,
    SimDev_W25Q32,
    SimDev_W25Q64,
    SimDev_W25Q128,
    SimDev_All,
} SimDevW25Qxx_Type_List;

typedef enum
{
    SimDevW25Qxx_Ok = 0,
    SimDevW25Qxx_Error,
    SimDevW25Qxx_Busy,
    SimDevW25Qxx_TimeOut,
} SimDevW25Qxx_Error_List;

typedef struct
{
    SimDevW25Qxx_Type_List prod_type;
    const char *name;

    uint32_t start_addr;
    uint32_t flash_size;

    uint32_t block_size;
    uint32_t sector_size;
    uint16_t page_size;

    uint16_t block_num;
    uint16_t sector_num;
    uint16_t page_num;
} SimDev_Info_TypeDef;

typedef struct
{
    SimDevW25Qxx_Type_List type;
    SimDev_Info_TypeDef info;
    uint32_t (*systick)(void);
    uint32_t start_time;
    uint32_t opr_time;

    uint16_t (*bus_read)(uint32_t addr, uint8_t *p_data, uint16_t size, uint16_t time_out);
    uint16_t (*bus_write)(uint32_t addr, uint8_t *p_data, uint16_t size, uint16_t time_out);
} SimDevW25QxxObj_TypeDef;

typedef struct
{
    SimDevW25Qxx_Error_List (*init)(SimDevW25QxxObj_TypeDef *dev);
    SimDevW25Qxx_Error_List (*write_sector)(SimDevW25QxxObj_TypeDef *dev, uint32_t addr, uint8_t *tx, uint32_t size);
    SimDevW25Qxx_Error_List (*read_sector)(SimDevW25QxxObj_TypeDef *dev, uint32_t addr, uint8_t *rx, uint32_t size);
    SimDevW25Qxx_Error_List (*erase_sector)(SimDevW25QxxObj_TypeDef *dev, uint32_t addr);
    SimDevW25Qxx_Error_List (*erase_chip)(SimDevW25QxxObj_TypeDef *dev);
    SimDev_Info_TypeDef (*info)(SimDevW25QxxObj_TypeDef *dev);
    int32_t (*get_section_start_addr)(SimDevW25QxxObj_TypeDef *dev, uint32_t addr);
} SimDevW25Qxx_TypeDef;

extern SimDevW25Qxx_TypeDef SimDevW25Qxx;

#endif
