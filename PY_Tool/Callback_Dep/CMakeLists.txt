cmake_minimum_required(VERSION 3.24)
project(Module)
find_package(Python3 COMPONENTS  Development REQUIRED)
add_library(StorageModule MODULE ui_callback.c)
message("python include dir : " ${Python3_INCLUDE_DIRS})
message("python version     : " ${Python3_VERSION})
target_include_directories(StorageModule PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(StorageModule PRIVATE ${Python3_LIBRARIES})
if (WIN32)
    # on windows platform depends on visual studio c++
    set(MODULE_SUFFIX ".pyd")
else()
    set(MODULE_SUFFIX ".so")
endif()
set_target_properties(StorageModule PROPERTIES 
                      PREFIX ""
                      SUFFIX ${MODULE_SUFFIX})