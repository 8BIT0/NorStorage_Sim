#include <Python.h>
#include <unistd.h>
#include "../../Dep/util.h"
#include "../../Storage_Port/Storage.h"

#define CALLBACK_TAG "Visual Callback"
#define CALLBACK_PRINT(stage, fmt, ...) Debug_Print(CALLBACK_TAG, stage, fmt, ##__VA_ARGS__) 

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

static struct PyModuleDef Module = {
    PyModuleDef_HEAD_INIT,
    "StorageModule",
    "Callback to Storage Create Search Modify Delete",
    -1,
    Storage_Callback
};

PyMODINIT_FUNC PyInit_StorageModule_Callback(void) {
    return PyModule_Create(&Module);
}

/* widget callback */
static PyObject* Ceate_StorageItem(PyObject* self, PyObject* args)
{
    CALLBACK_PRINT("callback", "Create");
    return NULL;
}

static PyObject* Search_StorageItem(PyObject* self, PyObject* args)
{
    CALLBACK_PRINT("callback", "Search");
    return NULL;
}

static PyObject* Modify_StorageItem(PyObject* self, PyObject* args)
{
    CALLBACK_PRINT("callback", "Modify");
    return NULL;
}

static PyObject* Delete_StorageItem(PyObject* self, PyObject* args)
{
    CALLBACK_PRINT("callback", "Delete");
    return NULL;
}