from setuptools import setup, Extension

AcceleratedDots = Extension('AcceleratedDots',
                            sources=['_accelerated-dots.c', '_backend-functions.c'])

setup(name='AcceleratedDots',
    version='1.0', 
    description='WORK DAMN YOU',
    ext_modules=[AcceleratedDots] )
