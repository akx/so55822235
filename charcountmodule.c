#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string.h>
#include <stdio.h>

static unsigned int count(const char *str, const char *key) {
  unsigned int n = 0;
  char * pch = strpbrk (str, key);
  while (pch != NULL)
  {
  	n++;
    pch = strpbrk (pch + 1, key);
  }
  return n;
}

static PyObject *
charcount_count(PyObject *self, PyObject *args)
{
	const char *str, *key;
	Py_ssize_t strl, keyl;
    
    if (!PyArg_ParseTuple(args, "s#s#", &str, &strl, &key, &keyl)) {
    	PyErr_SetString(PyExc_RuntimeError, "invalid arguments");
        return NULL;
    }
    //printf("%p %d %p %d\n", str, strl, key, keyl);
    int n = count(str, key);
    return PyLong_FromLong(n);
}

static PyMethodDef CharCountMethods[] = {
    {"count", charcount_count, METH_VARARGS, "Count the total occurrences of any s2 characters in s1"},
    {NULL, NULL, 0, NULL},
};

static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "charcount",
    NULL,
    -1,
    CharCountMethods
};

PyMODINIT_FUNC
PyInit_charcount(void)
{
    return PyModule_Create(&spammodule);
}