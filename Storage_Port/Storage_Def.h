#ifndef __STORAGE_DEF_H
#define __STORAGE_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"

#define Flash_Storage_TabSize           (4 Kb)

#define Storage_TabSize                 Flash_Storage_TabSize
#define Storage_InfoPageSize            Flash_Storage_TabSize

#define Storage_Assert(x)               while(x)

#define Format_Retry_Cnt                5
#define Module_ReInit_Cnt               5

#define Storage_ErrorCode_ToStr(x)      #x

#define From_Start_Address              0

#define Storage_ReserveBlock_Size       (4 Kb)

#define Storage_Item_Capacity           (1 Kb)

#define EXTERNAL_STORAGE_PAGE_TAG       "[Storage]"
#define EXTERNAL_PAGE_TAG_SIZE          strlen(EXTERNAL_STORAGE_PAGE_TAG)

#define STORAGE_NAME_LEN                41
#define STORAGE_ITEM_HEAD_TAG           0xAA
#define STORAGE_ITEM_END_TAG            0xBB
#define STORAGE_SLOT_HEAD_TAG           0xEF0110FE
#define STORAGE_SLOT_END_TAG            0xFE1001EF
#define STORAGE_DATA_ALIGN              4
#define STORAGE_MIN_BYTE_SIZE           1
#define STORAGE_FREEITEM_NAME           "Item_Avaliable"

#define Flash_Start_Addr                (1 Mb)
#define Flash_Storage_DefaultData       0xFF
#define Flash_Storage_ResData           0x55
#define Flash_Storage_TotalSize         (8 Mb)

#define FLash_ReserveSec_Size           (4 Kb)
#define Flash_SysDataSec_Size           (64 Kb)
#define Flash_UserDataSec_Size          (3 Mb)

#ifdef __cplusplus
}
#endif

#endif
