#include <Python.h>
#include <structmember.h>
#include <sys/timerfd.h>

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    int fd;
} Py_timerfd;

static void
Py_timerfd_dealloc(Py_timerfd* self)
{
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static int
Py_timerfd_init(Py_timerfd *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"clockid", "flags", NULL};
    int clockid, flags = 0;

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "i|i", kwlist, &clockid, &flags))
        return -1;

    self->fd = timerfd_create(clockid, flags);
    if(self->fd == -1) {
        PyErr_SetFromErrno(PyExc_OSError);
        return -1;
    }
    return 0;
}


static PyMemberDef Py_timerfd_members[] = {
    {"fd", T_INT, offsetof(Py_timerfd, fd), 0, "The timer file descriptor"},
    {NULL}  /* Sentinel */
};

static PyObject *
Py_timerfd_settime(Py_timerfd *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"flags", "new_value", NULL};
    int flags;
    struct itimerspec new_value = {0}, old_value = {0};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "i(ilil)", kwlist, &flags,
            &new_value.it_interval.tv_sec,
            &new_value.it_interval.tv_nsec,
            &new_value.it_value.tv_sec,
            &new_value.it_value.tv_nsec))
        return NULL;

    if(timerfd_settime(self->fd, flags, &new_value, &old_value) == -1) {
        PyErr_SetFromErrno(PyExc_OSError);
        return NULL;
    }
    return Py_BuildValue(
        "ilil",
        old_value.it_interval.tv_sec, 
        old_value.it_interval.tv_nsec,
        old_value.it_value.tv_sec,
        old_value.it_value.tv_nsec);
}

static PyObject *
Py_timerfd_gettime(Py_timerfd *self)
{
    struct itimerspec curr_value = {0};

    if(timerfd_gettime(self->fd, &curr_value) == -1) {
        PyErr_SetFromErrno(PyExc_OSError);
        return NULL;
    }
    return Py_BuildValue(
        "ilil",
        curr_value.it_interval.tv_sec,
        curr_value.it_interval.tv_nsec,
        curr_value.it_value.tv_sec,
        curr_value.it_value.tv_nsec);
}

static PyMethodDef Py_timerfd_methods[] = {
    {"settime", (PyCFunction)Py_timerfd_settime, METH_VARARGS | METH_KEYWORDS, ""},
    {"gettime", (PyCFunction)Py_timerfd_gettime, METH_VARARGS | METH_KEYWORDS, ""},
    {NULL}  /* Sentinel */
};

static PyTypeObject Py_timerfdType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_timerfd._timerfd",             /*tp_name*/
    sizeof(Py_timerfd), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Py_timerfd_dealloc,/*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "_timerfd objects",           /* tp_doc */
    0,                     /* tp_traverse */
    0,                     /* tp_clear */
    0,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    Py_timerfd_methods,             /* tp_methods */
    Py_timerfd_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Py_timerfd_init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,                 /* tp_new */
};

static PyMethodDef module_methods[] = {
    {NULL}  /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_timerfd",
        NULL,
        0,
        module_methods,
        NULL,
        NULL,
        NULL,
        NULL
};
#endif

#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC
PyInit__timerfd(void)
#else
#ifndef PyMODINIT_FUNC  /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
init_timerfd(void)
#endif
{
    PyObject *module;
    if (PyType_Ready(&Py_timerfdType) < 0)
        return NULL;

#if PY_MAJOR_VERSION >= 3
    module = PyModule_Create(&moduledef);
#else
    module = Py_InitModule3("_timerfd", module_methods, "Internal");
#endif
    if(!module)
        return NULL;

    Py_INCREF(&Py_timerfdType);
    if(PyModule_AddObject(module, "_timerfd", (PyObject *)&Py_timerfdType) != 0)
        return NULL;

    PyModule_AddIntConstant(module, "CLOCK_REALTIME", CLOCK_REALTIME);
    PyModule_AddIntConstant(module, "CLOCK_MONOTONIC", CLOCK_MONOTONIC);
    PyModule_AddIntConstant(module, "TFD_NONBLOCK", TFD_NONBLOCK);
    PyModule_AddIntConstant(module, "TFD_CLOEXEC", TFD_CLOEXEC);
    PyModule_AddIntConstant(module, "TFD_TIMER_ABSTIME", TFD_TIMER_ABSTIME);
#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}
