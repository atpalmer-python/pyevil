#include <Python.h>


static PyObject *get_refcnt(PyObject *self, PyObject *obj) {
    Py_ssize_t refcnt = Py_REFCNT(obj);
    return PyLong_FromLong(refcnt);
}


static PyObject *int_copy(PyObject *self, PyObject *obj) {
    if(Py_TYPE(obj) != &PyLong_Type) {
        PyErr_SetString(PyExc_TypeError, "ints only, please");
        return NULL;
    }

    PyLongObject *longobj = (PyLongObject *)obj;

    Py_ssize_t size = Py_SIZE(longobj);

    PyLongObject *result = (PyLongObject *)_PyLong_New(size);

    for(int i = 0; i < size; ++i) {
        result->ob_digit[i] = longobj->ob_digit[i];
    }

    return (PyObject *)result;
}


static PyObject *int_mutate(PyObject *self, PyObject *args) {
    PyObject *target;
    PyObject *newval;

    if(!PyArg_ParseTuple(args, "OO", &target, &newval))
        return NULL;

    if(Py_TYPE(target) != &PyLong_Type || Py_TYPE(newval) != &PyLong_Type) {
        PyErr_SetString(PyExc_TypeError, "Must be type int");
        return NULL;
    }

    Py_SIZE(target) = Py_SIZE(newval);
    for(int i = 0; i < Py_SIZE(newval); ++i) {
        ((PyLongObject *)target)->ob_digit[i] = ((PyLongObject *)newval)->ob_digit[i];
    }

    Py_RETURN_NONE;
}

static PyObject *tuple_set_item(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject *tuple;
    PyObject *value;
    int index;

    static char *kwlist[] = {"tuple", "value", "index"};

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|$Oi", kwlist, &tuple, &value, &index))
        return NULL;

    if(index < 0 || index > Py_SIZE(tuple)) {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        return NULL;
    }

    /* temporarily spoof refcnt to beat PyTuple_SetItem sanity checking */
    Py_ssize_t refcnt = Py_REFCNT(tuple);
    Py_REFCNT(tuple) = 1;

    PyTuple_SetItem(tuple, index, value);
    Py_INCREF(value);

    Py_REFCNT(tuple) = refcnt;

    Py_RETURN_NONE;
}

struct PyMethodDef methods[] = {
    { "get_refcnt", get_refcnt, METH_O, "" },
    { "int_copy", int_copy, METH_O, "" },
    { "int_mutate", int_mutate, METH_VARARGS, "" },
    { "tuple_set_item", (PyCFunction)tuple_set_item, METH_VARARGS | METH_KEYWORDS, "" },
    { 0 },
};

struct PyModuleDef module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "bastardize",
    .m_doc = "",
    .m_size = 0,
    .m_methods = methods,
};

PyMODINIT_FUNC PyInit_bastardize(void) {
    return PyModule_Create(&module);
};