This repo contains some experiments regarding [this Stack Overflow question about a faster `strpbrk()` in Python](https://stackoverflow.com/questions/55822235/strpbrk-in-python).

My original [answer](https://stackoverflow.com/a/55822728/51685) is reproduced below.

The updated TL;DR is that 

* GNU libc has a fast `strpbrk()`, probably even faster if you have SSE4
* macOS's default `strpbrk()` is very slow
* Python's built-in `count()` is pretty fast too, but you need to run it 4 times for 4 characters

The wrapper and experiment code here is MIT licensed; the GNU bits are licensed under the LGPL.

To run, `pip install cython`, then `make`.

Newest results at time of writing:

```
original                       | 3.366711 sec / 1000 iter | 297 iter/s
unrolled                       | 3.332623 sec / 1000 iter | 300 iter/s
cython libc                    | 4.451539 sec / 1000 iter | 224 iter/s
cython glibc                   | 1.774841 sec / 1000 iter | 563 iter/s <- fast
c extension, bytes             | 4.485469 sec / 1000 iter | 222 iter/s
c extension, unicode           | 4.475643 sec / 1000 iter | 223 iter/s
c extension glib, bytes        | 1.590433 sec / 1000 iter | 628 iter/s
c extension glib, unicode      | 1.573829 sec / 1000 iter | 635 iter/s <- fastest
```

---


I may have gone a little overboard here, but the tl;dr is that is the original code is actually faster than (EDIT: macOS's) `strpbrk()`, but some `strpbrk()` implementations may be faster!

`str.count()` uses [this bundle of strange and beautiful magic](https://github.com/python/cpython/blob/master/Objects/stringlib/fastsearch.h) in its guts – no wonder it's fast.

The full code is available at https://github.com/akx/so55822235 

# Python code

These approaches are in pure Python, including OP's original

```python
def gc_characters_original(haystack):
    gc_characters = 0
    for c in ("c", "C", "g", "G"):
        gc_characters += haystack.count(c)
    return gc_characters


def gc_characters_counter(haystack):
    counter = Counter(haystack)
    return sum(counter.get(c, 0) for c in ["c", "C", "g", "G"])


def gc_characters_manual(haystack):
    gc_characters = 0
    for x in haystack:
        if x in ("c", "C", "g", "G"):
            gc_characters += 1
    return gc_characters


def gc_characters_iters(haystack):
    gc_characters = haystack.count("c") + haystack.count("C") + haystack.count("g") + haystack.count("G")
    return gc_characters
```

# Cython extension wrapping `strpbrk()`

```cython
from libc.string cimport strpbrk

cdef int _count(char* s, char *key):
    assert s is not NULL, "byte string value is NULL"
    cdef int n = 0
    cdef char* pch = strpbrk (s, key)
    while pch is not NULL:
        n += 1
        pch = strpbrk (pch + 1, key)
    return n

def count(s, key):
    return _count(s, key)
```

...

```python
def gc_characters_cython(haystack_bytes):
    return charcount_cython.count(haystack_bytes, b"cCgG")
```

# Handmade C extension wrapping `strpbrk()`

```c
#define PY_SSIZE_T_CLEAN
#include <Python.h>
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

static PyObject *charcount_count(PyObject *self, PyObject *args) {
  const char *str, *key;
  Py_ssize_t strl, keyl;

  if (!PyArg_ParseTuple(args, "s#s#", &str, &strl, &key, &keyl)) {
    PyErr_SetString(PyExc_RuntimeError, "invalid arguments");
    return NULL;
  }
  int n = count(str, key);
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
```
...
```python
def gc_characters_cext_b(haystack_bytes):
    return charcount.count(haystack_bytes, b"cCgG")


def gc_characters_cext_u(haystack):
    return charcount.count(haystack, "cCgG")
```

---

# Measurements

On my Mac, counting `cCgG` in an one-million-character string of random letters, i.e.

```python
haystack = "".join(random.choice(string.ascii_letters) for x in range(1_000_000))
haystack_bytes = haystack.encode()
print("original", timeit.timeit(lambda: gc_characters_original(haystack), number=100))
print("unrolled", timeit.timeit(lambda: gc_characters_iters(haystack), number=100))
print("cython", timeit.timeit(lambda: gc_characters_cython(haystack_bytes), number=100))
print("c extension, bytes", timeit.timeit(lambda: gc_characters_cext_b(haystack_bytes), number=100))
print("c extension, unicode", timeit.timeit(lambda: gc_characters_cext_u(haystack), number=100))
print("manual loop", timeit.timeit(lambda: gc_characters_manual(haystack), number=100))
print("counter", timeit.timeit(lambda: gc_characters_counter(haystack), number=100))
```
yields these results:
```
original               0.34033612700000004
unrolled               0.33661798900000006
cython                 0.6542106270000001
c extension, bytes     0.46668797900000003
c extension, unicode   0.4761082090000004
manual loop           11.625232557
counter                7.0389275090000005
```

So, unless the `strpbrk()` implementation in my mac's `libc` is horribly underpowered (EDIT: which it is), it's just best to use `.count()`.

# EDIT

I added [glibc's `strcspn()`/`strpbrk()`](https://github.com/bminor/glibc/blob/fe92a91f1e713ba4951bc6e6162f8803dc76b816/string/strcspn.c) and it's startlingly faster than [the näive version of `strpbrk()` shipped with macOS](https://github.com/practicalswift/osx/blob/4c9b4581f398ec2d64f102508346cb9e8d764fbc/src/libc/string/FreeBSD/strpbrk.c):

```
original                       0.329256
unrolled                       0.333872
cython                         0.433299
c extension, bytes             0.432552
c extension, unicode           0.437332
c extension glibc, bytes       0.169704 <-- new
c extension glibc, unicode     0.158153 <-- new
```

`glibc` also has SSE2 and SSE4 versions of the functions, which would probably be even faster still.