#include "./Vm.hpp"
#include "ByteInfo.hpp"
#include "Objects.hpp"
#include "StackFrame.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

Vm::Vm(G_Header Header){
	this->Header = Header;
	this->Bytes = this->Header.Bytes;
	this->Instruction_Pointer = 0;
}

void Vm::ExitHandler(uint8_t ErrCode){
	/*
		Rn we dont care and we make the os free all the memory
	*/
	exit(ErrCode);
}


void Vm::Gc_clean(){
	for(uint64_t i = 0; i < MainStack.Size; ++i){
		if(MainStack.Vars[i]){
			MainStack.Vars[i]->mark();
		}
	}

	for(auto& i : this->Stack){
		i->mark();
	}

	for(auto& i : this->Frames){
		for(uint64_t p = 0; p < i.Size; ++p){
			if(MainStack.Vars[p]){
				MainStack.Vars[p]->mark();
			}
		}
	}

	std::vector<Object*> Newobjs;
	for(auto& i : this->Objects){
		if(!i->Marked){
			i->del();
			switch(i->Kind){
				case ObjKind::NUMBER: delete static_cast<NumberObj*>(i); break;
				case ObjKind::BOOL: delete static_cast<BoolObj*>(i); break;
				case ObjKind::STRING: delete static_cast<StrObj*>(i); break;
				case ObjKind::NIL: delete static_cast<NilObj*>(i); break;
			}
		}else{
			i->Marked = false;
			Newobjs.push_back(i);
		}
	}
	this->Objects = Newobjs;
}

Object* Vm::pop(){
	if(this->Stack.empty()){
		std::cerr << "Vm: Stack underflow\n";
		this->ExitHandler(1);
	}
	Object* p = this->Stack.back(); this->Stack.pop_back();
	p->Marked = false;
	return p;
}

void Vm::Execute(){
	//Read and Execute
	
	//Imediatly parse the main stack frame 
	MainStack.Size = this->get<uint64_t>();
	MainStack.Vars = new Object*[MainStack.Size];
	for(uint64_t i = 0; i < MainStack.Size; ++i){
		std::memset(&MainStack.Vars[i], 0, sizeof(Object*));
	}

	while(this->Header.size > this->Instruction_Pointer){
		if(this->Objects.size() > this->Last*2){
			this->Gc_clean();
			this->Last = this->Objects.size();
		}
		uint8_t Ins = this->get<uint8_t>();
		switch(static_cast<Instruction>(Ins)){
			case Instruction::MAKEFRAME:{
				StackFrame SF;
				SF.Size = this->get<uint64_t>();
				SF.Vars = new Object*[(uint8_t)SF.Size];
				for(uint64_t i = 0; i < SF.Size; ++i){
					std::memset(&SF.Vars[i], 0, sizeof(Object*));
				}
				this->Frames.push_back(SF);
				break;
			}
			case Instruction::PUSH:{
				switch(static_cast<PushType>(this->get<uint8_t>())){
					case PushType::NUMBER:{
						Object* Num = new NumberObj(this->get<double>());
						this->Stack.push_back(Num);
						this->Objects.push_back(Num);
						break;
					}	
				}
				break;
			}
			case Instruction::ADD:{
				Object* Operand1 = this->pop();
				Object* Operand2 = this->pop();				
				Object* Result = Operand1->add(Operand2);
				if(Result == nullptr){
					std::cerr << "Vm: No overload for " << Operand1->Kind << " + " << Operand2->Kind << "\n";
					this->ExitHandler(1);
				}
				this->Stack.push_back(Result);
				this->Objects.push_back(Result);
				break;
			}
			case Instruction::CMP_EQ:{
				Object* Operand1 = this->pop();
				Object* Operand2 = this->pop();				
				Object* Result = Operand1->cmp_eq(Operand2);
				if(Result == nullptr){
					std::cerr << "Vm: No overload for " << Operand1->Kind << " == " << Operand2->Kind << "\n";
					this->ExitHandler(1);
				}
				this->Stack.push_back(Result);
				this->Objects.push_back(Result);
				break;
			}
			case Instruction::CMP_NE:{
				Object* Operand1 = this->pop();
				Object* Operand2 = this->pop();				
				Object* Result = Operand1->cmp_ne(Operand2);
				if(Result == nullptr){
					std::cerr << "Vm: No overload for " << Operand1->Kind << " != " << Operand2->Kind << "\n";
					this->ExitHandler(1);
				}
				this->Stack.push_back(Result);
				this->Objects.push_back(Result);
				break;
			}
			case Instruction::CMP_LT:{
				Object* Operand1 = this->pop();
				Object* Operand2 = this->pop();				
				Object* Result = Operand1->cmp_lt(Operand2);
				if(Result == nullptr){
					std::cerr << "Vm: No overload for " << Operand1->Kind << " < " << Operand2->Kind << "\n";
					this->ExitHandler(1);
				}
				this->Stack.push_back(Result);
				this->Objects.push_back(Result);
				break;
			}
			case Instruction::CMP_LTE:{
				Object* Operand1 = this->pop();
				Object* Operand2 = this->pop();				
				Object* Result = Operand1->cmp_lte(Operand2);
				if(Result == nullptr){
					std::cerr << "Vm: No overload for " << Operand1->Kind << " <= " << Operand2->Kind << "\n";
					this->ExitHandler(1);
				}
				this->Stack.push_back(Result);
				this->Objects.push_back(Result);
				break;
			}
			case Instruction::CMP_MT:{
				Object* Operand1 = this->pop();
				Object* Operand2 = this->pop();				
				Object* Result = Operand1->cmp_mt(Operand2);
				if(Result == nullptr){
					std::cerr << "Vm: No overload for " << Operand1->Kind << " > " << Operand2->Kind << "\n";
					this->ExitHandler(1);
				}
				this->Stack.push_back(Result);
				this->Objects.push_back(Result);
				break;
			}
			case Instruction::CMP_MTE:{
				Object* Operand1 = this->pop();
				Object* Operand2 = this->pop();				
				Object* Result = Operand1->cmp_mte(Operand2);
				if(Result == nullptr){
					std::cerr << "Vm: No overload for " << Operand1->Kind << " >= " << Operand2->Kind << "\n";
					this->ExitHandler(1);
				}
				this->Stack.push_back(Result);
				this->Objects.push_back(Result);
				break;
			}
			case Instruction::PRINT:{
				Object* Obj = this->pop();
				StrObj* Representation = Obj->repr();
				this->Objects.push_back(Representation);
				std::cout << Representation->Value << "\n";
				break;
			}
			case Instruction::JMP:{
				uint64_t Pos = this->get<uint64_t>();
				this->Instruction_Pointer = Pos; 
				break;
			}
			case Instruction::JMP_IF_TRUE:{
				uint64_t Pos = this->get<uint64_t>();
				Object* Obj = this->pop();
				if(Obj->Kind != ObjKind::BOOL){
					std::cerr << "Vm: expected a bool type to jump\n";
					this->ExitHandler(1);
				} 
				if(static_cast<BoolObj*>(Obj)->Value == true){
					this->Instruction_Pointer = Pos; 
				}
				break;
			}
			case Instruction::JMP_IF_FALSE:{
				uint64_t Pos = this->get<uint64_t>();
				Object* Obj = this->pop();
				if(Obj->Kind != ObjKind::BOOL){
					std::cerr << "Vm: expected a bool type to jump\n";
					this->ExitHandler(1);
				} 
				if(static_cast<BoolObj*>(Obj)->Value == false){
					this->Instruction_Pointer = Pos; 
				}
				break;
			}
			case Instruction::CALL:{
				uint64_t pos = this->get<uint64_t>();
				this->RetStack.push_back(this->Instruction_Pointer);
				this->Instruction_Pointer = pos;
				break;
			}
			case Instruction::POP_CALL:{
				Object* Obj = this->pop();
				if(Obj->Kind != ObjKind::NUMBER){
					std::cerr << "Vm: expected a number type to call\n";
					this->ExitHandler(1);
				}
				this->RetStack.push_back(this->Instruction_Pointer);
				this->Instruction_Pointer = static_cast<NumberObj*>(Obj)->Value;
				break;
			}
			case Instruction::G_STORE:{
				uint64_t Id = this->get<uint64_t>();
				Object* Value = this->pop();
				this->MainStack.Vars[Id] = Value;
				break;
			}
			case Instruction::L_STORE:{
				uint64_t Id = this->get<uint64_t>();
				Object* Value = this->pop();
				if(this->Frames.empty()){
					std::cerr << "Vm: No stack frames\n";
					this->ExitHandler(1);
				}
				this->Frames.back().Vars[Id] = Value;
				break;
			}
			case Instruction::G_LOAD:{
				uint64_t Id = this->get<uint64_t>();
				this->Stack.push_back(this->MainStack.Vars[Id]);
				break;
			}
			case Instruction::L_LOAD:{
				uint64_t Id = this->get<uint64_t>();
				if(this->Frames.empty()){
					std::cerr << "Vm: No stack frames\n";
					this->ExitHandler(1);
				}
				this->Stack.push_back(this->Frames.back().Vars[Id]);
				break;
			}
			case Instruction::RET:{
				delete []this->Frames.back().Vars; 
				this->Frames.pop_back();
				this->Instruction_Pointer = this->RetStack.back(); this->RetStack.pop_back();
				break;
			}
		}
	}
}
	
