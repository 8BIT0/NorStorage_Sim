import os
import subprocess
import platform
from ConstDef import *

def Debug_Print(stage, *args):
    tag_str = '[ Compile ' + stage + ' ]'
    tag_size = len(tag_str)
    if tag_size < PRINT_ALIGN_LEN:
        tag_str += ' ' * (PRINT_ALIGN_LEN - tag_size)

    print(tag_str + " ".join(str(arg) for arg in args))

def compile():
    source_dir = os.getcwd() + os.path.sep + 'PY_Tool' + os.path.sep + 'Callback_Dep' + os.path.sep
    build_dir = os.path.join(source_dir, 'build')

    Debug_Print('file dir', source_dir)
    Debug_Print('build dir', build_dir)
    
    if os.path.exists(build_dir):
        Debug_Print('build dir', 'exist')
        if os.system(f'rm -rf {build_dir}') == 0:
            Debug_Print('build dir', 'Success')
        else:
            Debug_Print('build dir', 'Delete failed')
            return False

    os.mkdir(build_dir)
    if os.path.exists(build_dir):
        Debug_Print('build dir', 'Create success')
    else:
        Debug_Print('build dir', 'Create failed')
        return False

    try:
        os.chdir(build_dir)

        sys = platform.system()
        
        print('--------------------------------- compiling -----------------------------------------------')
        print('-------------------------------------------------------------------------------------------')
        if sys == 'Darwin':
            cmake_command = f'cmake ..'
            subprocess.run(cmake_command, shell=True, check=True)

            cmake_command = f'make'
            subprocess.run(cmake_command, shell=True, check=True)
        elif sys == 'win':
            cmake_command = f'cmake -G Ninja ..'
            subprocess.run(cmake_command, shell=True, check=True)

        print('-------------------------------------------------------------------------------------------')
        print('-------------------------------------------------------------------------------------------')
        Debug_Print('state', 'Success')
        return True
    except subprocess.CalledProcessError as e:
        Debug_Print('state', 'Failed', e)
        return False
    except Exception as e:
        Debug_Print('state', 'Failed unknow')
        return False