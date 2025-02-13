from distutils.core import setup, Extension

module = Extension('Storage', sources=['py_dsp_tool.c'])

setup(name='Storage',
      version='1.0',
      description='Storage callback module',
      ext_modules=[module])
