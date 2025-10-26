#include "./Objects.hpp"
#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>

void Object::mark(){};
void Object::del(){};

Object* Object::add(Object* other){return nullptr;};
Object* Object::cmp_eq(Object* other){return nullptr;};
Object* Object::cmp_ne(Object* other){return nullptr;};
Object* Object::cmp_lt(Object* other){return nullptr;};
Object* Object::cmp_lte(Object* other){return nullptr;};
Object* Object::cmp_mt(Object* other){return nullptr;};
Object* Object::cmp_mte(Object* other){return nullptr;};
StrObj* Object::repr(){return nullptr;};

void NilObj::mark(){};
void NilObj::del(){};

Object* NilObj::add(Object* other){return nullptr;};
Object* NilObj::cmp_eq(Object* other){return nullptr;};
Object* NilObj::cmp_ne(Object* other){return nullptr;};
Object* NilObj::cmp_lt(Object* other){return nullptr;};
Object* NilObj::cmp_lte(Object* other){return nullptr;};
Object* NilObj::cmp_mt(Object* other){return nullptr;};
Object* NilObj::cmp_mte(Object* other){return nullptr;};
StrObj* NilObj::repr(){
	return new StrObj("nil");
}

void NumberObj::del(){};
NumberObj::NumberObj(double Value){
	this->Value = Value;
	this->Kind = ObjKind::NUMBER;
	this->Marked = false;
}

void NumberObj::mark(){
	this->Marked = true;
}

Object* NumberObj::add(Object* other){
	if(other->Kind == ObjKind::NUMBER){
		return new NumberObj(static_cast<NumberObj*>(other)->Value + this->Value);
	}
	return nullptr;
}

Object* NumberObj::cmp_eq(Object* other){
	if(other->Kind == ObjKind::NUMBER){
		
		return new BoolObj(static_cast<NumberObj*>(other)->Value == this->Value);
	}
	return nullptr;
}

Object* NumberObj::cmp_ne(Object* other){
	if(other->Kind == ObjKind::NUMBER){
		
		return new BoolObj(static_cast<NumberObj*>(other)->Value != this->Value);
	}
	return nullptr;
}

Object* NumberObj::cmp_lt(Object* other){
	if(other->Kind == ObjKind::NUMBER){
		
		return new BoolObj(static_cast<NumberObj*>(other)->Value < this->Value);
	}
	return nullptr;
}

Object* NumberObj::cmp_lte(Object* other){
	if(other->Kind == ObjKind::NUMBER){
		
		return new BoolObj(static_cast<NumberObj*>(other)->Value <= this->Value);
	}
	return nullptr;
}

Object* NumberObj::cmp_mt(Object* other){
	if(other->Kind == ObjKind::NUMBER){
		
		return new BoolObj(static_cast<NumberObj*>(other)->Value > this->Value);
	}
	return nullptr;
}


Object* NumberObj::cmp_mte(Object* other){
	if(other->Kind == ObjKind::NUMBER){
		
		return new BoolObj(static_cast<NumberObj*>(other)->Value >= this->Value);
	}
	return nullptr;
}

StrObj* NumberObj::repr(){
	std::ostringstream oss;
	oss << std::setprecision(std::numeric_limits<double>::digits10) << this->Value << "\n";
	std::string str =  oss.str();
	return new StrObj(str);
}


BoolObj::BoolObj(bool Value){
	this->Value = Value;
	this->Marked = false;
	this->Kind = ObjKind::BOOL;
}

void BoolObj::del(){};
void BoolObj::mark(){
	this->Marked = true;
}

Object* BoolObj::cmp_eq(Object* other){
	if(other->Kind == ObjKind::BOOL){
		
		return new BoolObj(static_cast<BoolObj*>(other)->Value == this->Value);
	}
	return nullptr;
}

Object* BoolObj::cmp_ne(Object* other){
	if(other->Kind == ObjKind::BOOL){
		
		return new BoolObj(static_cast<BoolObj*>(other)->Value != this->Value);
	}
	return nullptr;
}

StrObj* BoolObj::repr(){
	return new StrObj(this->Value ? "True" : "False");
}

Object* BoolObj::add(Object* other){return nullptr;};
Object* BoolObj::cmp_lt(Object* other){return nullptr;};
Object* BoolObj::cmp_lte(Object* other){return nullptr;};
Object* BoolObj::cmp_mt(Object* other){return nullptr;};
Object* BoolObj::cmp_mte(Object* other){return nullptr;};


StrObj::StrObj(std::string Value){
	this->Kind = ObjKind::STRING;
	this->Value =  Value;
	this->Marked = false;
}

void StrObj::del(){}
void StrObj::mark(){
	this->Marked = true;
}

Object* StrObj::add(Object* other){
	if(other->Kind == ObjKind::STRING){
		
		return new StrObj(static_cast<StrObj*>(other)->Value + this->Value);
	}
	return nullptr;
}

Object* StrObj::cmp_eq(Object* other){
	if(other->Kind == ObjKind::STRING){
		
		return new BoolObj(static_cast<StrObj*>(other)->Value == this->Value);
	}
	return nullptr;
}

Object* StrObj::cmp_ne(Object* other){
	if(other->Kind == ObjKind::STRING){
		
		return new BoolObj(static_cast<StrObj*>(other)->Value != this->Value);
	}
	return nullptr;
}

StrObj* StrObj::repr(){
	return this;
}
Object* StrObj::cmp_lt(Object* other){return nullptr;};
Object* StrObj::cmp_lte(Object* other){return nullptr;};
Object* StrObj::cmp_mt(Object* other){return nullptr;};
Object* StrObj::cmp_mte(Object* other){return nullptr;};


std::ostream& operator<<(std::ostream& os, ObjKind Kind){
	switch(Kind){
		case ObjKind::NUMBER: return os << "number";
		case ObjKind::BOOL: return os << "bool";
		case ObjKind::NIL: return os << "nil";
		case ObjKind::STRING: return os << "string";
		default: return os;
	}
}
