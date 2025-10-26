#pragma once 

#include <cstddef>
#include "./Objects.hpp"

struct StackFrame{
	//Max amount of variables 
	size_t Size;
	//This is a list of all our variables 
	Object** Vars;
};
