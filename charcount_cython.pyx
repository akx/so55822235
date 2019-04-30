# distutils: include_dirs = .
from libc.string cimport strpbrk

cdef extern from "./glibc-string.h":
    char *glibc_strpbrk(const char *s, const char *accept)


cdef int _libc_count(char* s, char *key):
    cdef int n = 0
    cdef char* pch = strpbrk (s, key)
    while pch is not NULL:
        n += 1
        pch = strpbrk (pch + 1, key)
    return n


cdef int _glibc_count(char* s, char *key):
    cdef int n = 0
    cdef char* pch = glibc_strpbrk (s, key)
    while pch is not NULL:
        n += 1
        pch = glibc_strpbrk (pch + 1, key)
    return n


def libc_count(s, key):
    return _libc_count(s, key)


def glibc_count(s, key):
    return _glibc_count(s, key)