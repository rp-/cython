
//////////////////// ArgTypeTest.proto ////////////////////

static int __Pyx_ArgTypeTest(PyObject *obj, PyTypeObject *type, int none_allowed,
    const char *name, int exact); /*proto*/

//////////////////// ArgTypeTest ////////////////////

static int __Pyx_ArgTypeTest(PyObject *obj, PyTypeObject *type, int none_allowed,
    const char *name, int exact)
{
    if (!type) {
        PyErr_Format(PyExc_SystemError, "Missing type object");
        return 0;
    }
    if (none_allowed && obj == Py_None) return 1;
    else if (exact) {
        if (Py_TYPE(obj) == type) return 1;
    }
    else {
        if (PyObject_TypeCheck(obj, type)) return 1;
    }
    PyErr_Format(PyExc_TypeError,
        "Argument '%s' has incorrect type (expected %s, got %s)",
        name, type->tp_name, Py_TYPE(obj)->tp_name);
    return 0;
}

//////////////////// RaiseArgTupleInvalid.proto ////////////////////

static void __Pyx_RaiseArgtupleInvalid(const char* func_name, int exact,
    Py_ssize_t num_min, Py_ssize_t num_max, Py_ssize_t num_found); /*proto*/

//////////////////// RaiseArgTupleInvalid ////////////////////

//  __Pyx_RaiseArgtupleInvalid raises the correct exception when too
//  many or too few positional arguments were found.  This handles
//  Py_ssize_t formatting correctly.

static void __Pyx_RaiseArgtupleInvalid(
    const char* func_name,
    int exact,
    Py_ssize_t num_min,
    Py_ssize_t num_max,
    Py_ssize_t num_found)
{
    Py_ssize_t num_expected;
    const char *more_or_less;

    if (num_found < num_min) {
        num_expected = num_min;
        more_or_less = "at least";
    } else {
        num_expected = num_max;
        more_or_less = "at most";
    }
    if (exact) {
        more_or_less = "exactly";
    }
    PyErr_Format(PyExc_TypeError,
                 "%s() takes %s %" PY_FORMAT_SIZE_T "d positional argument%s (%" PY_FORMAT_SIZE_T "d given)",
                 func_name, more_or_less, num_expected,
                 (num_expected == 1) ? "" : "s", num_found);
}


//////////////////// RaiseKeywordRequired.proto ////////////////////

static CYTHON_INLINE void __Pyx_RaiseKeywordRequired(const char* func_name, PyObject* kw_name); /*proto*/

//////////////////// RaiseKeywordRequired ////////////////////

static CYTHON_INLINE void __Pyx_RaiseKeywordRequired(
    const char* func_name,
    PyObject* kw_name)
{
    PyErr_Format(PyExc_TypeError,
        #if PY_MAJOR_VERSION >= 3
        "%s() needs keyword-only argument %U", func_name, kw_name);
        #else
        "%s() needs keyword-only argument %s", func_name,
        PyString_AS_STRING(kw_name));
        #endif
}


//////////////////// RaiseDoubleKeywords.proto ////////////////////

static void __Pyx_RaiseDoubleKeywordsError(const char* func_name, PyObject* kw_name); /*proto*/

//////////////////// RaiseDoubleKeywords ////////////////////

static void __Pyx_RaiseDoubleKeywordsError(
    const char* func_name,
    PyObject* kw_name)
{
    PyErr_Format(PyExc_TypeError,
        #if PY_MAJOR_VERSION >= 3
        "%s() got multiple values for keyword argument '%U'", func_name, kw_name);
        #else
        "%s() got multiple values for keyword argument '%s'", func_name,
        PyString_AS_STRING(kw_name));
        #endif
}


//////////////////// KeywordStringCheck.proto ////////////////////

static CYTHON_INLINE int __Pyx_CheckKeywordStrings(PyObject *kwdict, const char* function_name, int kw_allowed); /*proto*/

//////////////////// KeywordStringCheck ////////////////////

//  __Pyx_CheckKeywordStrings raises an error if non-string keywords
//  were passed to a function, or if any keywords were passed to a
//  function that does not accept them.

static CYTHON_INLINE int __Pyx_CheckKeywordStrings(
    PyObject *kwdict,
    const char* function_name,
    int kw_allowed)
{
    PyObject* key = 0;
    Py_ssize_t pos = 0;
    while (PyDict_Next(kwdict, &pos, &key, 0)) {
        #if PY_MAJOR_VERSION < 3
        if (unlikely(!PyString_CheckExact(key)) && unlikely(!PyString_Check(key)))
        #else
        if (unlikely(!PyUnicode_Check(key)))
        #endif
            goto invalid_keyword_type;
    }
    if ((!kw_allowed) && unlikely(key))
        goto invalid_keyword;
    return 1;
invalid_keyword_type:
    PyErr_Format(PyExc_TypeError,
        "%s() keywords must be strings", function_name);
    return 0;
invalid_keyword:
    PyErr_Format(PyExc_TypeError,
    #if PY_MAJOR_VERSION < 3
        "%s() got an unexpected keyword argument '%s'",
        function_name, PyString_AsString(key));
    #else
        "%s() got an unexpected keyword argument '%U'",
        function_name, key);
    #endif
    return 0;
}


//////////////////// ParseKeywords.proto ////////////////////

static int __Pyx_ParseOptionalKeywords(PyObject *kwds, PyObject **argnames[], \
    PyObject *kwds2, PyObject *values[], Py_ssize_t num_pos_args, \
    const char* function_name); /*proto*/

//////////////////// ParseKeywords ////////////////////
//@requires: RaiseDoubleKeywords

//  __Pyx_ParseOptionalKeywords copies the optional/unknown keyword
//  arguments from the kwds dict into kwds2.  If kwds2 is NULL, unknown
//  keywords will raise an invalid keyword error.
//
//  Three kinds of errors are checked: 1) non-string keywords, 2)
//  unexpected keywords and 3) overlap with positional arguments.
//
//  If num_posargs is greater 0, it denotes the number of positional
//  arguments that were passed and that must therefore not appear
//  amongst the keywords as well.
//
//  This method does not check for required keyword arguments.

static int __Pyx_ParseOptionalKeywords(
    PyObject *kwds,
    PyObject **argnames[],
    PyObject *kwds2,
    PyObject *values[],
    Py_ssize_t num_pos_args,
    const char* function_name)
{
    PyObject *key = 0, *value = 0;
    Py_ssize_t pos = 0;
    PyObject*** name;
    PyObject*** first_kw_arg = argnames + num_pos_args;

    while (PyDict_Next(kwds, &pos, &key, &value)) {
        name = first_kw_arg;
        while (*name && (**name != key)) name++;
        if (*name) {
            values[name-argnames] = value;
        } else {
            #if PY_MAJOR_VERSION < 3
            if (unlikely(!PyString_CheckExact(key)) && unlikely(!PyString_Check(key))) {
            #else
            if (unlikely(!PyUnicode_Check(key))) {
            #endif
                goto invalid_keyword_type;
            } else {
                for (name = first_kw_arg; *name; name++) {
                    #if PY_MAJOR_VERSION >= 3
                    if (PyUnicode_GET_SIZE(**name) == PyUnicode_GET_SIZE(key) &&
                        PyUnicode_Compare(**name, key) == 0) break;
                    #else
                    if (PyString_GET_SIZE(**name) == PyString_GET_SIZE(key) &&
                        _PyString_Eq(**name, key)) break;
                    #endif
                }
                if (*name) {
                    values[name-argnames] = value;
                } else {
                    /* unexpected keyword found */
                    for (name=argnames; name != first_kw_arg; name++) {
                        if (**name == key) goto arg_passed_twice;
                        #if PY_MAJOR_VERSION >= 3
                        if (PyUnicode_GET_SIZE(**name) == PyUnicode_GET_SIZE(key) &&
                            PyUnicode_Compare(**name, key) == 0) goto arg_passed_twice;
                        #else
                        if (PyString_GET_SIZE(**name) == PyString_GET_SIZE(key) &&
                            _PyString_Eq(**name, key)) goto arg_passed_twice;
                        #endif
                    }
                    if (kwds2) {
                        if (unlikely(PyDict_SetItem(kwds2, key, value))) goto bad;
                    } else {
                        goto invalid_keyword;
                    }
                }
            }
        }
    }
    return 0;
arg_passed_twice:
    __Pyx_RaiseDoubleKeywordsError(function_name, **name);
    goto bad;
invalid_keyword_type:
    PyErr_Format(PyExc_TypeError,
        "%s() keywords must be strings", function_name);
    goto bad;
invalid_keyword:
    PyErr_Format(PyExc_TypeError,
    #if PY_MAJOR_VERSION < 3
        "%s() got an unexpected keyword argument '%s'",
        function_name, PyString_AsString(key));
    #else
        "%s() got an unexpected keyword argument '%U'",
        function_name, key);
    #endif
bad:
    return -1;
}
