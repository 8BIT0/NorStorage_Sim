#include <Python.h>
#include "py_dsp_tool.h"
#include "../Dep/util.h"


#define VISUAL_TAG "Visual Module"
#define VISUAL_PRINT(stage, fmt, ...) Debug_Print(VISUAL_TAG, stage, fmt, ##__VA_ARGS__) 

typedef struct
{
    PyObject *p_ModuleName;
    PyObject *p_Module;
    PyObject *p_Dict;
    PyObject *p_Class;
} VisualMonitor_TypeDef;

/* internal vriable */
static VisualMonitor_TypeDef MonitorObj;

/* external function */
static bool PyDspTool_Init(char *simfile_dir, char *file_name);

PyDsp_TypeDef PY_Visualize = {
    .init = PyDspTool_Init,
};

static bool PyDspTool_Init(char *simfile_dir, char *file_name)
{
    memset(&MonitorObj, 0, sizeof(VisualMonitor_TypeDef));
    Py_Initialize();

    if (!Py_IsInitialized() || \
        (simfile_dir == NULL) || \
        (file_name == NULL) || \
        (strlen(simfile_dir) == 0) || \
        (strlen(file_name) == 0))
        return false;

    /* import module */
    if (((MonitorObj.p_ModuleName = PyUnicode_DecodeFSDefault("StructureDisplay")) == NULL) || \
        ((MonitorObj.p_Module = PyImport_Import(MonitorObj.p_ModuleName)) == NULL))
    {
        PyErr_Print();
        return false;
    }

    Py_DECREF(MonitorObj.p_ModuleName);
    if ((MonitorObj.p_Dict = PyModule_GetDict(MonitorObj.p_Module)) != NULL)
    {
        MonitorObj.p_Class = PyDict_GetItemString(MonitorObj.p_Dict, "StructureDisplay");
        if (!PyCallable_Check(MonitorObj.p_Class))
        {
            return false;
        }
    }


    return true;
}

