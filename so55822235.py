import string
import random
import timeit
import charcount_cython
import charcount
from collections import Counter


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


def gc_characters_cython_libc(haystack_bytes):
    return charcount_cython.libc_count(haystack_bytes, b"cCgG")


def gc_characters_cython_glibc(haystack_bytes):
    return charcount_cython.glibc_count(haystack_bytes, b"cCgG")


def gc_characters_cython_fast(haystack_bytes):
    return charcount_cython.fast_count(haystack_bytes, b"cCgG")



def gc_characters_cext_b(haystack_bytes):
    return charcount.count(haystack_bytes, b"cCgG", 0)


def gc_characters_cext_u(haystack):
    return charcount.count(haystack, "cCgG", 0)


def gc_characters_cext_gb(haystack_bytes):
    return charcount.count(haystack_bytes, b"cCgG", 1)


def gc_characters_cext_gu(haystack):
    return charcount.count(haystack, "cCgG", 1)


def gc_characters_cext_xb(haystack_bytes):
    return charcount.count(haystack_bytes, b"cCgG", 2)


def gc_characters_cext_xu(haystack):
    return charcount.count(haystack, "cCgG", 2)


def time_assert(name, func, number=2000):
    assert func() == check_n
    t = timeit.timeit(func, number=number)
    itps = number / t
    print("%-30s | %.6f sec / %4d iter | %d iter/s" % (name, t, number, itps))


print("generating data...")
haystack = "".join(random.choice(string.ascii_letters) for x in range(1_000_000))
haystack_bytes = haystack.encode()
check_n = gc_characters_original(haystack)

time_assert("original", lambda: gc_characters_original(haystack))
time_assert("unrolled", lambda: gc_characters_iters(haystack))
time_assert("cython libc", lambda: gc_characters_cython_libc(haystack_bytes))
time_assert("cython glibc", lambda: gc_characters_cython_glibc(haystack_bytes))
time_assert("cython fast", lambda: gc_characters_cython_fast(haystack_bytes))
time_assert("c extension, bytes", lambda: gc_characters_cext_b(haystack_bytes))
time_assert("c extension, unicode", lambda: gc_characters_cext_u(haystack))
time_assert("c extension glib, bytes", lambda: gc_characters_cext_gb(haystack_bytes))
time_assert("c extension glib, unicode", lambda: gc_characters_cext_gu(haystack))
time_assert("c extension fast, bytes", lambda: gc_characters_cext_xb(haystack_bytes))
time_assert("c extension fast, unicode", lambda: gc_characters_cext_xu(haystack))

#time_assert("manual loop", lambda: gc_characters_manual(haystack))
#time_assert("counter", lambda: gc_characters_counter(haystack))
