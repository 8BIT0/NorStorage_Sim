#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "SimDataFile_Opr.h"

/* internal function */
static bool SimModule_Init(void);

int main(int arg)
{
    SimDataFileObj_TypeDef obj;
    memset(&obj, 0, sizeof(SimDataFileObj_TypeDef));
    obj.malloc = malloc;
    obj.free = free;
    
    SimDataFile.create(&obj, "test", 1);
    
    SimModule_Init();
    return 0;
}

static bool SimModule_Init(void)
{
    int8_t num = -1;

    while (num < 0)
    {
        scanf("%d", &num);
    }
}

static bool Create_SimData()
{

}
