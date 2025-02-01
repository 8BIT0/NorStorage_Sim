#include <Python.h>
#include <unistd.h>
#include "py_dsp_tool.h"
#include "../Dep/util.h"

#define VISUAL_TAG "Visual Module"
#define VISUAL_PRINT(stage, fmt, ...) Debug_Print(VISUAL_TAG, stage, fmt, ##__VA_ARGS__) 

typedef struct
{
    PyObject *p_Name;
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
    char py_file_path[1024] = {'\0'};
    PyObject *prj_path = NULL;
    
    if (getcwd(py_file_path, 1024) == NULL)
        return false;
    
    strcat(py_file_path, Folder_Split);
    strcat(py_file_path, "PY_Tool");

    if ((prj_path = PySys_GetObject((const char *)"path")) == NULL)
        return false;

    PyList_Append(prj_path, PyUnicode_DecodeFSDefault((const char *)py_file_path));

    memset(&MonitorObj, 0, sizeof(VisualMonitor_TypeDef));
    Py_Initialize();

    if (!Py_IsInitialized() || \
        (simfile_dir == NULL) || \
        (file_name == NULL) || \
        (strlen(simfile_dir) == 0) || \
        (strlen(file_name) == 0))
        return false;

    /* import module */
    if (((MonitorObj.p_Name = PyUnicode_DecodeFSDefault("StructureDisplay")) == NULL) || \
        ((MonitorObj.p_Module = PyImport_Import(MonitorObj.p_Name)) == NULL))
    {
        PyErr_Print();
        return false;
    }

    Py_DECREF(MonitorObj.p_Name);
    if ((MonitorObj.p_Dict = PyModule_GetDict(MonitorObj.p_Module)) != NULL)
    {
        MonitorObj.p_Class = PyDict_GetItemString(MonitorObj.p_Dict, "StructureDisplay");
        if (!PyCallable_Check(MonitorObj.p_Class))
        {
            return false;
        }
    }

    return false;
}

