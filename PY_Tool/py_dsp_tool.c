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
    PyObject *p_Class;
    PyObject *p_Instance;
} VisualMonitor_TypeDef;

/* internal vriable */
static VisualMonitor_TypeDef MonitorObj;

/* external function */
static bool PyDspTool_Init(char *simfile_dir, char *file_name, uint32_t addr_offset);

PyDsp_TypeDef PY_Visualize = {
    .init = PyDspTool_Init,
};

static bool PyDspTool_Init(char *simfile_dir, char *file_name, uint32_t addr_offset)
{
    char py_file_path[1024] = {'\0'};
    PyObject *prj_path = NULL;
    memset(&MonitorObj, 0, sizeof(VisualMonitor_TypeDef));
    
    if (getcwd(py_file_path, 1024) == NULL)
    {
        VISUAL_PRINT("Get file path", "Failed");
        return false;
    }

    strcat(py_file_path, Folder_Split);
    strcat(py_file_path, "PY_Tool");

    Py_Initialize();

    if (!Py_IsInitialized() || \
        (simfile_dir == NULL) || \
        (file_name == NULL) || \
        (strlen(simfile_dir) == 0) || \
        (strlen(file_name) == 0))
    {
        VISUAL_PRINT("Parameter", "Error");
        return false;
    }

    if ((prj_path = PySys_GetObject((const char *)"path")) == NULL)
    {
        VISUAL_PRINT("Get PySys Obj", "Failed");
        return false;
    }

    PyList_Append(prj_path, PyUnicode_DecodeFSDefault((const char *)py_file_path));
    
    /* import module */
    if (((MonitorObj.p_Name = PyUnicode_DecodeFSDefault("StructureDisplay")) == NULL) || \
        ((MonitorObj.p_Module = PyImport_Import(MonitorObj.p_Name)) == NULL))
    {
        PyErr_Print();
        return false;
    }

    Py_DECREF(MonitorObj.p_Name);
    MonitorObj.p_Class = PyObject_GetAttrString(MonitorObj.p_Module, "StructureDisplay");
    if ((MonitorObj.p_Class == NULL) || (PyCallable_Check(MonitorObj.p_Class) == 0))
    {
        VISUAL_PRINT("Class check", "Uncallable");
        return false;
    }

    /* init python class */
    PyObject *p_ClassArg = Py_BuildValue("ssI", simfile_dir, file_name, addr_offset);
    if (p_ClassArg == NULL)
        return false;
    
    MonitorObj.p_Instance = PyObject_CallObject(MonitorObj.p_Class, p_ClassArg);
    Py_DECREF(p_ClassArg);

    if (MonitorObj.p_Instance == NULL)
        return false;

    /* link dll or so */

    PyObject_CallMethod(MonitorObj.p_Instance, "show_widget", NULL);

    return true;
}


