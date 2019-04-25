from distutils.core import setup, Extension

module1 = Extension(
    "charcount",
    sources=["charcountmodule.c"],
    extra_compile_args = ["-mtune=native"],
)

setup(name="so55822235", version="0.0", ext_modules=[module1])
