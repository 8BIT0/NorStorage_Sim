import os
import subprocess
import platform

def compile():
    source_dir = os.getcwd() + os.path.sep + 'PY_Tool' + os.path.sep + 'Callback_Dep' + os.path.sep
    build_dir = os.path.join(source_dir, 'build')

    print("[ Storage Compile file dir ] ", source_dir)
    print("[ Storage Compile build dir ] ", build_dir)
    
    if os.path.exists(build_dir):
        print("[ Storage Compile build dir ] exists")
        if os.system(f'rm -rf {build_dir}') == 0:
            print("[ Storage Compile build dir ] delete success")
        else:
            print("[ Storage Compile build dir ] delete failed")
            return False

    os.mkdir(build_dir)
    if os.path.exists(build_dir):
        print("[ Storage Compile build dir ] create success")
    else:
        print("[ Storage Compile build dir ] create failed")
        return False

    try:
        os.chdir(build_dir)

        sys = platform.system()
        
        if sys == 'Darwin':
            cmake_command = f'cmake ..'
            subprocess.run(cmake_command, shell=True, check=True)

            cmake_command = f'make'
            subprocess.run(cmake_command, shell=True, check=True)
        elif sys == 'win':
            cmake_command = f'cmake -G Ninja ..'
            subprocess.run(cmake_command, shell=True, check=True)

        print("[ Storage Compile ] Success")
        return True
    except subprocess.CalledProcessError as e:
        print(f"[ Storage Compile ] Failed", e)
        return False
    except Exception as e:
        print(f"[ Storage Compile ] Failed", "Unknow Error Occur")
        return False