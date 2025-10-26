#pragma once 
#include <cstdint>

struct G_Header{
	uint64_t size; 
	uint8_t* Bytes; 
};

enum class PushType : uint8_t{
	NUMBER,
};

enum class Instruction : uint8_t{
	MAKEFRAME, //0 
	PRINT, // 1

	PUSH, // 2
	ADD, // 3

	CMP_EQ, //4
	CMP_NE, //5

	CMP_LT,//6 
	CMP_LTE,//7 

	CMP_MT, //8 
	CMP_MTE, //9 
	
	JMP, //10 
	JMP_IF_TRUE, //11
	JMP_IF_FALSE, //12

	CALL, //13 
	POP_CALL, //14 
	RET, //15 

	G_STORE, //16
	L_STORE, //17 

	G_LOAD, //18 
	L_LOAD, //19
};
