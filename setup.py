#!/usr/bin/env python

"""
setup.py file for progress module
"""

from distutils.core import setup, Extension


progress_module = Extension('_progress',
                           sources=['progress_wrap.c', 'progress.c'],
                           )

setup(name='progress',
       version='0.1',
       author="Gaurav Atreya",
       description="""Module to show multiline progress status.""",
       ext_modules=[progress_module],
       py_modules=["progress"],
       )
