from setuptools import setup, Extension


setup(
    name='pyevil',
    ext_modules=[Extension('pyevil', sources=['src/pyevil.c'])]
)
