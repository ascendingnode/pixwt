from distutils.core import setup
from Cython.Build import cythonize

# The Cython build call
setup(
        name = "pixwt",
        ext_modules = cythonize(('pixwt.pyx')),
        )
