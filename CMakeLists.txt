cmake_minimum_required(VERSION 4.1.2)

#chose a project name(this will not be  the name of the exe)
project(GVM LANGUAGES CXX)

#standard
set(CMAKE_CXX_STANDARD 20)

add_compile_options(-g -Wall -Wextra -O3 -s -g) #compile options here isnt it o3 and not 03

set(GVM_HEADER_FILES
    ./ByteVM/instructions.hpp
    ./ByteVM/object.h
    ./ByteVM/iobytes.hpp
    ./ByteVM/header.h
)

#adsd ??
set(GVM_SOURCE_FILES
    ./ByteVM/GVM.cpp
)

set(GASM_HEADER_FILES
    ./ByteVM/assembler/assembler.hpp
    ./ByteVM/iobytes.hpp
    ./ByteVM/object.h
    ./ByteVM/instructions.hpp
    ./ByteVM/header.h
)

set(GASM_SOURCE_FILES
    ./ByteVM/assembler/assembler.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#exe name and shit
add_executable(assembler ${GASM_HEADER_FILES} ${GASM_SOURCE_FILES})
add_executable(gvm ${GVM_HEADER_FILES} ${GVM_SOURCE_FILES})

#linking here like lm for math etc
#target_link_libraries()
