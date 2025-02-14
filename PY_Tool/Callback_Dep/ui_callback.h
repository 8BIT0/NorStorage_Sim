#ifndef __UI_CALLBACK_H
#define __UI_CALLBACK_H

#include <stdint.h>
#include <string.h>

typedef enum
{
    Trigger_Create = 0,
    Trigger_Modify,
    Trigger_Delete,
    Trigger_Search,
    Trigger_None,
} TriggerType_List;

typedef enum
{
    UserSec = 0,
    SysSec,
} Sec_List;

typedef struct
{
    Sec_List sec;
    TriggerType_List type;
    char *name;
    uint8_t *data;
    uint16_t size;
} TriggerData_TypeDef;

typedef void (*trigger_callback)(TriggerData_TypeDef *data);

void UICallback_Set(TriggerType_List type, trigger_callback cb);

void UICallback_Create(Sec_List type, char *name, uint8_t *data, uint16_t len);
void UICallback_Search(Sec_List type, char *name, uint8_t *data, uint16_t len);
void UICallback_Modify(Sec_List type, char *name, uint8_t *data, uint16_t len);
void UICallback_Delete(Sec_List type, char *name, uint8_t *data, uint16_t len);

#endif


