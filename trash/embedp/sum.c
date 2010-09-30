#include "Python.h"

PyObject* sum_pair(PyObject* self, PyObject* args) {
  PyObject *result = NULL;

  long a,b;

  if (PyArg_ParseTuple(args, "ii", &a, &b)) {
    result = Py_BuildValue("i", a+b);
  }
  return result;
}

PyMethodDef methods[] = {
  {"sum_pair", sum_pair, METH_VARARGS},
  {NULL, NULL},
};

void initsum() {
  (void) Py_InitModule3("sum", methods,
			"Sum two numbers and such.");
}

