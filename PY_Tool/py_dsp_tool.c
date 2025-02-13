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
} VisualMonitor_TypeDef;

/* internal vriable */
static VisualMonitor_TypeDef MonitorObj;

/* internal function */
static PyObject* Ceate_StorageItem(PyObject* self, PyObject* args);
static PyObject* Search_StorageItem(PyObject* self, PyObject* args);
static PyObject* Modify_StorageItem(PyObject* self, PyObject* args);
static PyObject* Delete_StorageItem(PyObject* self, PyObject* args);

static PyMethodDef Storage_Callback[] = {
    {"create_callback", Ceate_StorageItem, METH_VARARGS, NULL},
    {"search_callback", Search_StorageItem, METH_VARARGS, NULL},
    {"modify_callback", Modify_StorageItem, METH_VARARGS, NULL},
    {"delete_callback", Delete_StorageItem, METH_VARARGS, NULL},
};

static struct PyModuleDef StorageModule = {
    PyModuleDef_HEAD_INIT,
    "Storage",
    "Callback to Storage Create Search Modify Delete",
    -1,
    Storage_Callback
};

/* external function */
static bool PyDspTool_Init(char *simfile_dir, char *file_name, uint32_t addr_offset);

PyDsp_TypeDef PY_Visualize = {
    .init = PyDspTool_Init,
};


PyMODINIT_FUNC PyInit_StorageModule_Callback(void) {
    return PyModule_Create(&StorageModule);
}

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
    
    PyObject_CallObject(MonitorObj.p_Class, p_ClassArg);
    Py_DECREF(p_ClassArg);

    return true;
}

/* widget callback */
static PyObject* Ceate_StorageItem(PyObject* self, PyObject* args)
{
    VISUAL_PRINT("callback", "Create");
    return NULL;
}

static PyObject* Search_StorageItem(PyObject* self, PyObject* args)
{
    VISUAL_PRINT("callback", "Search");
    return NULL;
}

static PyObject* Modify_StorageItem(PyObject* self, PyObject* args)
{
    VISUAL_PRINT("callback", "Modify");
    return NULL;
}

static PyObject* Delete_StorageItem(PyObject* self, PyObject* args)
{
    VISUAL_PRINT("callback", "Delete");
    return NULL;
}

