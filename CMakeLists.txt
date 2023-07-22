cmake_minimum_required(VERSION 3.15)
project(dload)

set(CMAKE_CXX_STANDARD 11)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/module)

add_library(module1 SHARED base.h module_src/module1.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

add_executable(dload main.cpp)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(dload ${CMAKE_DL_LIBS})
endif ()

