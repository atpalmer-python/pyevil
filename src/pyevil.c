#include <Python.h>


static PyObject *
int_copy(PyObject *self, PyObject *obj)
{
    if(Py_TYPE(obj) != &PyLong_Type) {
        PyErr_SetString(PyExc_TypeError, "ints only, please");
        return NULL;
    }

    PyLongObject *longobj = (PyLongObject *)obj;

    Py_ssize_t size = Py_SIZE(longobj);

    /*
     * Python abuses the sign bit on PyVarObject's ob_size field to represent negative ints.
     * The "size" variable above may actually be negative, confusing the audience...
     * Do weird stuff here to deal with Python's weird stuff.
     */

    Py_ssize_t realsize = size < 0 ? (-size) : size;
    PyLongObject *result = (PyLongObject *)_PyLong_New(realsize);

    Py_SIZE(result) = Py_SIZE(longobj); /* copy the sign bit, not just the real size */
    for(int i = 0; i < realsize; ++i) {
        result->ob_digit[i] = longobj->ob_digit[i];
    }

    return (PyObject *)result;
}


static PyObject *
int_mutate(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *target;
    PyObject *newval;
    int force = 0;

    static char *kwlist[] = {"target", "newval", "force", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|$p", kwlist, &target, &newval, &force)) {
        return NULL;
    }

    if(Py_TYPE(target) != &PyLong_Type || Py_TYPE(newval) != &PyLong_Type) {
        PyErr_SetString(PyExc_TypeError, "Must be type int");
        return NULL;
    }

    if(!force) {
        if(Py_REFCNT(target) > 3) {
            /*
             * If you just made a copy, you'll probably have refcnt=3.
             * More is suspect...
             */
            char message[256];
            snprintf(message, 256,
                "The refcnt of %ld is already %lu, so you don't want to do this... "
                "Or just set force=True to get rid of this error. What could go wrong?",
                PyLong_AsLong(target), Py_REFCNT(target));
            PyErr_SetString(PyExc_ValueError, message);
            return NULL;
        }
    }

    Py_SIZE(target) = Py_SIZE(newval);
    for(int i = 0; i < Py_SIZE(newval); ++i) {
        ((PyLongObject *)target)->ob_digit[i] = ((PyLongObject *)newval)->ob_digit[i];
    }

    Py_RETURN_NONE;
}

static PyObject *
tuple_set_item(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *tuple;
    PyObject *value;
    int index;

    static char *kwlist[] = {"tuple", "value", "index", NULL};

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

static PyObject *float_mutate(PyObject *self, PyObject *args) {
    PyObject *target;
    double newval;

    if(!PyArg_ParseTuple(args, "Od", &target, &newval))
        return NULL;

    if(!PyFloat_Check(target)) {
        PyErr_SetString(PyExc_TypeError, "float_mutate needs a float");
        return NULL;
    }

    ((PyFloatObject *)target)->ob_fval = newval;

    Py_RETURN_NONE;
}

static PyObject *float_copy(PyObject *self, PyObject *obj) {
    if(!PyFloat_Check(obj)) {
        PyErr_SetString(PyExc_TypeError, "Try again with a float next time...");
        return NULL;
    }

    PyObject *result = PyObject_MALLOC(sizeof(PyFloatObject));
    PyObject_INIT(result, &PyFloat_Type);
    ((PyFloatObject *)result)->ob_fval = ((PyFloatObject *)obj)->ob_fval;
    return result;
}

struct PyMethodDef methods[] = {
    { "int_copy", int_copy, METH_O, "" },
    { "int_mutate", (PyCFunction)int_mutate, METH_VARARGS | METH_KEYWORDS, "" },
    { "float_copy", float_copy, METH_O, "" },
    { "float_mutate", float_mutate, METH_VARARGS, "" },
    { "tuple_set_item", (PyCFunction)tuple_set_item, METH_VARARGS | METH_KEYWORDS, "" },
    { 0 },
};

struct PyModuleDef module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "pyevil",
    .m_doc = "",
    .m_size = 0,
    .m_methods = methods,
};

PyMODINIT_FUNC PyInit_pyevil(void) {
    return PyModule_Create(&module);
};
