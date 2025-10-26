cmake_minimum_required(VERSION 4.0.0)

project(GVM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -O3) 


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

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(gvm ${GVM_HEADER_FILES} ${GVM_SOURCE_FILES})

target_link_libraries(gvm)
