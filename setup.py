#!/usr/bin/env python

"""
setup.py file for progress module
"""

from distutils.core import setup, Extension


c_progress_module = Extension('_c_progress',
                           sources=['c_progress_wrap.c', 'c_progress.c'],
                           )

setup(name='c_progress',
       version='0.1',
       author="Gaurav Atreya",
       description="""Module to show multiline progress status.""",
       ext_modules=[c_progress_module],
       py_modules=["c_progress"],
       )
