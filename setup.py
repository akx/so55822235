from distutils.core import setup, Extension
from Cython.Build import cythonize

extensions = cythonize([
    Extension(
        "charcount",
        sources=["charcountmodule.c"],
        extra_compile_args = ["-mtune=native"],
    ),
    Extension(
        "charcount_cython",
        sources=["charcount_cython.pyx"],
    ),
])


setup(name="so55822235", version="0.0", ext_modules=extensions)
