#pragma once 
#include "ByteInfo.hpp"
#include "Objects.hpp"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stack>
#include <vector>
#include "./StackFrame.hpp"

class Vm{
	G_Header Header;
	uint64_t Instruction_Pointer;
	uint8_t* Bytes;

	StackFrame MainStack;
	std::vector<Object*> Objects;
	
	/*
	 Yo anything on the stack is not managed by the gc 
	 push 8  <- not managed by the gc 

	 when you do something like "add" it frees the two elemnts at the top
	 there should NEVER be any value on the stack! all values must eneter a variable or function at some point! 
	 */
	std::vector<Object*> Stack;
	uint64_t Last = 200;

	std::vector<uint64_t> RetStack;
	std::vector<StackFrame> Frames;


public:
	Vm(G_Header Header);
	void Execute();
	void ExitHandler(uint8_t ErrCode);
	
	Object* pop();

	template<typename T>
	T get(){
		if(this->Instruction_Pointer + sizeof(T) > this->Header.size){
			std::cerr << "Vm: Invalid byte size: " << this->Instruction_Pointer << ":" << sizeof(T) << "\n"; 
			this->ExitHandler(1);
		}
		T Val;
		std::memcpy(&Val, &this->Bytes[this->Instruction_Pointer], sizeof(T));
		this->Instruction_Pointer+=sizeof(T);
		return Val;
	}
	
	void Gc_clean();
};
