#ifndef __SYSCOMMON_H
#define __SYSCOMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#if defined WIN
#include <windows.h>

#define Sleep_Ms(x) Sleep(x)
typedef DWORD WINAPI (*Thread_Callback)(LPVOID *arg);
#else
#include <pthread.h>

#define Sleep_Ms(x) usleep(x * 1000)
typedef void* Thread_Callback;
#endif

typedef void* Thread_Hdl;

typedef struct
{
    uint32_t size;
    uint8_t *p_buf;
} Stream_TypeDef;

Thread_Hdl Sys_CreateThread(Thread_Callback cb);
void Sys_CancelThread(Thread_Hdl hdl);

void* Sys_Malloc(uint32_t size);
void Sys_Free(void **ptr);

#endif
