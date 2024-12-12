#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "SimDataFile_Opr.h"

/* internal function */
static bool SimModule_Init(void);

int main(int argc, char **argv)
{
    SimDataFileObj_TypeDef obj;
    memset(&obj, 0, sizeof(SimDataFileObj_TypeDef));
    obj.malloc = malloc;
    obj.free = free;
    
    SimDataFile.create(&obj, argv[0], "test", 1);

    SimModule_Init();
    return 0;
}

static bool SimModule_Init(void)
{
    int num = -1;

    // while (num < 0)
    // {
    //     scanf("%d", &num);
    // }

    return false;
}


