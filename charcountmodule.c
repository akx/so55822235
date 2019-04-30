#define PY_SSIZE_T_CLEAN
#include "glibc-string.h"
#include <Python.h>
#include <stdio.h>
#include <string.h>

static unsigned int count(const char *str, const char *key) {
  unsigned int n = 0;
  char *pch = strpbrk(str, key);
  while (pch != NULL) {
    n++;
    pch = strpbrk(pch + 1, key);
  }
  return n;
}

static unsigned int gcount(const char *str, const char *key) {
  unsigned int n = 0;
  char *pch = glibc_strpbrk(str, key);
  while (pch != NULL) {
    n++;
    pch = glibc_strpbrk(pch + 1, key);
  }
  return n;
}

static PyObject *charcount_count(PyObject *self, PyObject *args) {
  const char *str, *key;
  Py_ssize_t strl, keyl;
  int mode;

  if (!PyArg_ParseTuple(args, "s#s#i", &str, &strl, &key, &keyl, &mode)) {
    PyErr_SetString(PyExc_RuntimeError, "invalid arguments");
    return NULL;
  }
  // printf("%p %d %p %d\n", str, strl, key, keyl);
  int n = -1;
  switch (mode) {
  case 0:
    n = count(str, key);
    break;
  case 1:
    n = gcount(str, key);
    break;
  }
  return PyLong_FromLong(n);
}

static PyMethodDef CharCountMethods[] = {
    {"count", charcount_count, METH_VARARGS,
     "Count the total occurrences of any s2 characters in s1"},
    {NULL, NULL, 0, NULL},
};

static struct PyModuleDef spammodule = {PyModuleDef_HEAD_INIT, "charcount",
                                        NULL, -1, CharCountMethods};

PyMODINIT_FUNC PyInit_charcount(void) { return PyModule_Create(&spammodule); }