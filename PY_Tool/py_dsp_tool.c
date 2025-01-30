#include <Python.h>
#include "py_dsp_tool.h"

/* external function */
static bool PyDspTool_Init(void);

PyDsp_TypeDef PY_Visualize = {
    .init = PyDspTool_Init,
};

static bool PyDspTool_Init(void)
{
    Py_Initialize();
}

