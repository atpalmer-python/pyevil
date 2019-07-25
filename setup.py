from setuptools import setup, Extension


setup(
    name='bastardize',
    ext_modules=[ Extension('bastardize', sources=[ 'src/bastardize.c' ]) ]
)
