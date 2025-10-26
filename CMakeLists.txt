cmake_minimum_required(VERSION 4.0.0)


#chose a project name(this will not be  the name of the exe)
project(GVM LANGUAGES CXX)

#standard
set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -O3) #compile options here isnt it o3 and not 03

set(GVM_HEADER_FILES
	./vm/ByteInfo.hpp
	./vm/Vm.hpp
	./vm/Objects.hpp
	./vm/StackFrame.hpp
)
set(GVM_SOURCE_FILES
  ./vm/main.cpp
	./vm/Vm.cpp
	./vm/Objects.cpp
	./vm/StackFrame.cpp
)

#set(GASM_HEADER_FILES
#)

#set(GASM_SOURCE_FILES
#)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#exe name and shit
#add_executable(assembler ${GASM_HEADER_FILES} ${GASM_SOURCE_FILES})
add_executable(gvm ${GVM_HEADER_FILES} ${GVM_SOURCE_FILES})

target_link_libraries(gvm)
#linking here like lm for math2 etc
