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