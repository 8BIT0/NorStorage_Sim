from distutils.core import setup
from Cython.Build import cythonize
from distutils.extension import Extension

extensions = [
    Extension(
        "storage_callback",
        sources=["c:\\Users\\HUAV\\Desktop\\8B!T0\\NorSim_Storage\\PY_Tool\\link_callback.pyx", "callback.c"],
    )
]

setup(
    name="storage module",
    ext_modules=cythonize(extensions),
)