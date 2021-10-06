cmake_minimum_required(VERSION 3.13)
project(Purple C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

file (GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.c")

add_compile_options(-D_FILE_OFFSET_BITS=64)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(purple ${SRC_FILES})
