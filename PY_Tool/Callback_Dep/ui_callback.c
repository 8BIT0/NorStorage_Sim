#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ui_callback.h"

static trigger_callback create_cb = NULL;
static trigger_callback search_cb = NULL;
static trigger_callback delete_cb = NULL;
static trigger_callback modify_cb = NULL;

void UICallback_Set(TriggerType_List type, trigger_callback cb)
{
    switch ((uint8_t)type)
    {
        case Trigger_Create: create_cb = cb; break;
        case Trigger_Modify: modify_cb = cb; break;
        case Trigger_Delete: delete_cb = cb; break;
        case Trigger_Search: search_cb = cb; break;
        defautl: break;
    }
}

void UICallback_Create(Sec_List type, char *name, uint8_t *data, uint16_t len)
{
    TriggerData_TypeDef data_tmp;

    printf("create name %s len %d\r\n", name, len);
    memset(&data_tmp, 0, sizeof(TriggerData_TypeDef));
    data_tmp.sec = type;
    data_tmp.type = Trigger_Create;
    data_tmp.name = malloc(strlen(name));
    if (data_tmp.name == NULL)
    {
        printf("create callback name malloc failed\r\n");
        return;
    }

    memset(data_tmp.name, 0, sizeof(strlen(name)));
    strcpy(data_tmp.name, name);
    data_tmp.data = malloc(len);
    if (data_tmp.data == NULL)
    {
        printf("create callback data malloc failed\r\n");
        free(data_tmp.name);
        return;
    }
    
    memset(data_tmp.data, 0, len);
    memcpy(data_tmp.data, data, len);

    if (create_cb != NULL)
        create_cb(&data_tmp);

    free(data_tmp.name);
    free(data_tmp.data);
}

void UICallback_Search(Sec_List type, char *name, uint8_t *data, uint16_t len)
{
    TriggerData_TypeDef data_tmp;

    printf("search name %s len %d\r\n", name, len);
    memset(&data_tmp, 0, sizeof(TriggerData_TypeDef));
    data_tmp.sec = type;
    data_tmp.type = Trigger_Create;
    data_tmp.name = malloc(strlen(name));
    if (data_tmp.name == NULL)
    {
        printf("search callback name malloc failed\r\n");
        return;
    }

    memset(data_tmp.name, 0, sizeof(strlen(name)));
    strcpy(data_tmp.name, name);
    data_tmp.data = malloc(len);
    if (data_tmp.data == NULL)
    {
        printf("search callback data malloc failed\r\n");
        free(data_tmp.name);
        return;
    }
    
    memset(data_tmp.data, 0, len);
    memcpy(data_tmp.data, data, len);

    if (search_cb != NULL)
        search_cb(&data_tmp);

    free(data_tmp.name);
    free(data_tmp.data);
}

void UICallback_Modify(Sec_List type, char *name, uint8_t *data, uint16_t len)
{
    TriggerData_TypeDef data_tmp;

    printf("modify name %s len %d\r\n", name, len);
    memset(&data_tmp, 0, sizeof(TriggerData_TypeDef));
    data_tmp.sec = type;
    data_tmp.type = Trigger_Create;
    data_tmp.name = malloc(strlen(name));
    if (data_tmp.name == NULL)
    {
        printf("modify callback name malloc failed\r\n");
        return;
    }

    memset(data_tmp.name, 0, sizeof(strlen(name)));
    strcpy(data_tmp.name, name);
    data_tmp.data = malloc(len);
    if (data_tmp.data == NULL)
    {
        printf("modify callback data malloc failed\r\n");
        free(data_tmp.name);
        return;
    }
    
    memset(data_tmp.data, 0, len);
    memcpy(data_tmp.data, data, len);

    if (modify_cb != NULL)
        modify_cb(&data_tmp);

    free(data_tmp.name);
    free(data_tmp.data);
}

void UICallback_Delete(Sec_List type, char *name, uint8_t *data, uint16_t len)
{
    TriggerData_TypeDef data_tmp;

    printf("delete name %s len %d\r\n", name, len);
    memset(&data_tmp, 0, sizeof(TriggerData_TypeDef));
    data_tmp.sec = type;
    data_tmp.type = Trigger_Create;
    data_tmp.name = malloc(strlen(name));
    if (data_tmp.name == NULL)
    {
        printf("delete callback name malloc failed\r\n");
        return;
    }

    memset(data_tmp.name, 0, sizeof(strlen(name)));
    strcpy(data_tmp.name, name);

    if (delete_cb != NULL)
        delete_cb(&data_tmp);

    free(data_tmp.name);
}

