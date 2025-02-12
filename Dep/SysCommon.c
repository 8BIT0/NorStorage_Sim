#include "SysCommon.h"
#include "util.h"

Thread_Hdl Sys_CreateThread(Thread_Callback cb)
{
    Thread_Hdl hdl_tmp = NULL;

    if (cb == NULL)
        return NULL;

#if defined WIN
    hdl_tmp = CreateThread(NULL, (8 Kb), (LPTHREAD_START_ROUTINE)cb, NULL, 0, NULL);
#else
    hdl_tmp = Sys_Malloc(sizeof(pthread_t));
    if (hdl_tmp == NULL)
        return NULL;

    if (pthread_create(hdl_tmp, NULL, cb, NULL) != 0)
    {
        /* free hdl_tmp pointer */
        Sys_Free(&hdl_tmp);
       return NULL; 
    }
#endif

    return hdl_tmp;
}

void Sys_CancelThread(Thread_Hdl hdl)
{
    if (hdl == NULL)
        return;

#if defined WIN
    CloseHandle((HANDLE)hdl);
#else
    pthread_cancel(*(pthread_t *)hdl);
    Sys_Free(&hdl);
#endif
}

void* Sys_Malloc(uint32_t size)
{
    void *ptr = NULL;
    ptr = malloc((size_t)size);

    if (ptr)
        memset(ptr, 0, size);

    return ptr;
}

void Sys_Free(void **ptr)
{
    if ((ptr == NULL) || (*ptr == NULL))
        return;

    free(*ptr);
    *ptr = NULL;
}
