#include <sys/time.h>
#include "Runtime.h"

typedef struct
{
    struct timeval time;
    SysTime_t start_ms;
    SysTime_t ms;
    bool init;
} RuntimeObj_TypeDef;

/* internal vriable define */
static RuntimeObj_TypeDef runtime_obj = {
    .init = false,
    .start_ms = 0,
};

/* external function */
static bool Runtime_Init(void);
static SysTime_t Runtime_GetMs(void);
static void Runtime_Tick(void);

Runtime_TypeDef Runtime = {
    .init = Runtime_Init,
    .get_ms = Runtime_GetMs,
    .tick = Runtime_Tick,
};

static bool Runtime_Init(void)
{
    /* get init system time */
    if (runtime_obj.init)
        return true;

    gettimeofday(&runtime_obj.time, NULL);
    runtime_obj.start_ms = runtime_obj.time.tv_sec * MS_PER_S;
    runtime_obj.start_ms += runtime_obj.time.tv_usec / US_PER_MS;
    runtime_obj.init = true;
    runtime_obj.ms = 0;

    return true;
}

static void Runtime_Tick(void)
{
    if (!runtime_obj.init)
        Runtime_Init();

    gettimeofday(&runtime_obj.time, NULL);
    runtime_obj.ms = runtime_obj.time.tv_sec * MS_PER_S;
    runtime_obj.ms += runtime_obj.time.tv_usec / US_PER_MS;
    runtime_obj.ms -= runtime_obj.start_ms;
}

static SysTime_t Runtime_GetMs(void)
{
    return runtime_obj.ms;
}
