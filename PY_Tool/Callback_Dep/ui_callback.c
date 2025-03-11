#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "ui_callback.h"

static trigger_callback create_cb = NULL;
static trigger_callback search_cb = NULL;
static trigger_callback delete_cb = NULL;
static trigger_callback modify_cb = NULL;

#define UI_CALLBACK_TAG "UI Callback"
#define DEBUG_BUF_SIZE 4096

#define CALLBACK_INFO(stage, fmt, ...) UIDebug_Print(UI_CALLBACK_TAG, stage, fmt, ##__VA_ARGS__)

static void UIDebug_Print(const char *tag, const char *stage, const char *fmt, ...)
{
    va_list ap;
    char fmt_buf[DEBUG_BUF_SIZE];
    char *ptr_tmp = NULL;

    memset(fmt_buf, '\0', DEBUG_BUF_SIZE);
    sprintf(fmt_buf, "[ %s %s ]", tag, stage);
    uint8_t align_size = 48 - strlen(fmt_buf);
    ptr_tmp = fmt_buf + strlen(fmt_buf);
    memset(ptr_tmp, ' ', align_size);
    ptr_tmp = ptr_tmp + align_size;

    va_start(ap, fmt);
    vsprintf(ptr_tmp, fmt, ap);
    strcat(ptr_tmp, "\r\n");
    va_end(ap);

    printf("%s", fmt_buf);
}

void UICallback_Set(TriggerType_List type, trigger_callback cb)
{
    switch ((uint8_t)type)
    {
        case Trigger_Create:
            create_cb = cb;
            CALLBACK_INFO("set", "Create callback");
            break;

        case Trigger_Modify:
            modify_cb = cb;
            CALLBACK_INFO("set", "Modify callback");
            break;

        case Trigger_Delete:
            delete_cb = cb;
            CALLBACK_INFO("set", "Delete callback");
            break;

        case Trigger_Search:
            search_cb = cb;
            CALLBACK_INFO("set", "Search callback");
            break;

        defautl: break;
    }
}

bool UICallback_Test(void)
{
    CALLBACK_INFO("", "test");
    return true;
}

bool UICallback_Create(Sec_List type, char *name, uint8_t *data, uint16_t len)
{
    bool state = false;
    TriggerData_TypeDef data_tmp;

    CALLBACK_INFO("create", "name %s data size %d", name, len);
    memset(&data_tmp, 0, sizeof(TriggerData_TypeDef));
    data_tmp.sec = type;
    data_tmp.type = Trigger_Create;
    data_tmp.name = malloc(strlen(name));
    if (data_tmp.name == NULL)
    {
        CALLBACK_INFO("create", "Name space malloc failed");
        return false;
    }

    memset(data_tmp.name, 0, sizeof(strlen(name)));
    strcpy(data_tmp.name, name);
    data_tmp.data = malloc(len);
    if (data_tmp.data == NULL)
    {
        CALLBACK_INFO("create", "Data space malloc failed");
        free(data_tmp.name);
        return false;
    }
    
    data_tmp.size = len;
    memset(data_tmp.data, 0, len);
    memcpy(data_tmp.data, data, len);

    if (create_cb != NULL)
        state = create_cb(&data_tmp);

    free(data_tmp.name);
    free(data_tmp.data);

    return state;
}

bool UICallback_Search(Sec_List type, char *name, uint8_t *data, uint16_t len)
{
    bool state = false;
    TriggerData_TypeDef data_tmp;

    CALLBACK_INFO("search", "Name %s data size %d", name, len);
    memset(&data_tmp, 0, sizeof(TriggerData_TypeDef));
    data_tmp.sec = type;
    data_tmp.type = Trigger_Create;
    data_tmp.name = malloc(strlen(name));
    if (data_tmp.name == NULL)
    {
        CALLBACK_INFO("search", "Name space malloc failed");
        return false;
    }

    memset(data_tmp.name, 0, sizeof(strlen(name)));
    strcpy(data_tmp.name, name);
    data_tmp.data = malloc(len);
    if (data_tmp.data == NULL)
    {
        CALLBACK_INFO("search", "Data space malloc failed");
        free(data_tmp.name);
        return false;
    }
    
    memset(data_tmp.data, 0, len);

    if (search_cb != NULL)
        state = search_cb(&data_tmp);

    free(data_tmp.name);
    free(data_tmp.data);

    return state;
}

bool UICallback_Modify(Sec_List type, char *name, uint8_t *data, uint16_t len)
{
    bool state = false;
    TriggerData_TypeDef data_tmp;

    CALLBACK_INFO("modify", "Name %s data size %d", name, len);
    memset(&data_tmp, 0, sizeof(TriggerData_TypeDef));
    data_tmp.sec = type;
    data_tmp.type = Trigger_Create;
    data_tmp.name = malloc(strlen(name));
    if (data_tmp.name == NULL)
    {
        CALLBACK_INFO("modify", "Data space malloc failed");
        return false;
    }

    memset(data_tmp.name, 0, sizeof(strlen(name)));
    strcpy(data_tmp.name, name);
    data_tmp.data = malloc(len);
    if (data_tmp.data == NULL)
    {
        CALLBACK_INFO("search", "Data space malloc failed");
        free(data_tmp.name);
        return false;
    }
    
    memset(data_tmp.data, 0, len);
    memcpy(data_tmp.data, data, len);

    if (modify_cb != NULL)
        state = modify_cb(&data_tmp);

    free(data_tmp.name);
    free(data_tmp.data);

    return state;
}

bool UICallback_Delete(Sec_List type, char *name, uint8_t *data, uint16_t len)
{
    bool state = false;
    TriggerData_TypeDef data_tmp;

    CALLBACK_INFO("delete", "Name %s data size %d", name, len);
    memset(&data_tmp, 0, sizeof(TriggerData_TypeDef));
    data_tmp.sec = type;
    data_tmp.type = Trigger_Create;
    data_tmp.name = malloc(strlen(name));
    if (data_tmp.name == NULL)
    {
        CALLBACK_INFO("delete", "Name space malloc failed");
        return false;
    }

    memset(data_tmp.name, 0, sizeof(strlen(name)));
    strcpy(data_tmp.name, name);

    if (delete_cb != NULL)
        state = delete_cb(&data_tmp);

    free(data_tmp.name);
    
    return state;
}

