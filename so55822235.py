import pyximport

pyximport.install()
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


def gc_characters_cython(haystack_bytes):
    return charcount_cython.count(haystack_bytes, b"cCgG")


def gc_characters_cext_b(haystack_bytes):
    return charcount.count(haystack_bytes, b"cCgG")


def gc_characters_cext_u(haystack):
    return charcount.count(haystack, "cCgG")


print("generating data...")
haystack = "".join(random.choice(string.ascii_letters) for x in range(1_000_000))
haystack_bytes = haystack.encode()
print("original", timeit.timeit(lambda: gc_characters_original(haystack), number=100))
print("unrolled", timeit.timeit(lambda: gc_characters_iters(haystack), number=100))
print("cython", timeit.timeit(lambda: gc_characters_cython(haystack_bytes), number=100))
print("c extension, bytes", timeit.timeit(lambda: gc_characters_cext_b(haystack_bytes), number=100))
print("c extension, unicode", timeit.timeit(lambda: gc_characters_cext_u(haystack), number=100))
print("manual loop", timeit.timeit(lambda: gc_characters_manual(haystack), number=100))
print("counter", timeit.timeit(lambda: gc_characters_counter(haystack), number=100))
