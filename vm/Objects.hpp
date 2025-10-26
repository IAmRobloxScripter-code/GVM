#pragma once 

#include <cstdint>
#include <ostream>

enum class ObjKind{
	NUMBER, 
	BOOL, 
	STRING, 
	NIL,
};
struct StrObj;

struct Object{
	bool Marked = false;
	ObjKind Kind; 

	virtual void mark();
	virtual void del();


	//whole lot of operators!
	virtual Object* add(Object* other);
	virtual Object* cmp_eq(Object* other);
	virtual Object* cmp_ne(Object* other);
	virtual Object* cmp_lt(Object* other);
	virtual Object* cmp_lte(Object* other);
	virtual Object* cmp_mt(Object* other);
	virtual Object* cmp_mte(Object* other);
	virtual StrObj* repr();
};

struct NumberObj : public Object{
	double Value;	
	NumberObj(double Value);
	void mark() override;
	void del() override;

	Object* add(Object* other) override;
	Object* cmp_eq(Object* other) override;
	Object* cmp_ne(Object* other) override;
	Object* cmp_lt(Object* other) override;
	Object* cmp_lte(Object* other) override;
	Object* cmp_mt(Object* other) override;
	Object* cmp_mte(Object* other) override;
	
	StrObj* repr() override;
};

struct BoolObj : public Object{
	bool Value;	
	BoolObj(bool Value);

	void mark() override;
	void del() override;

	Object* add(Object* other) override;
	Object* cmp_eq(Object* other) override;
	Object* cmp_ne(Object* other) override;
	Object* cmp_lt(Object* other) override;
	Object* cmp_lte(Object* other) override;
	Object* cmp_mt(Object* other) override;
	Object* cmp_mte(Object* other) override;

	StrObj* repr() override;
};

struct StrObj : public Object{
	std::string Value;	
	StrObj(std::string Value);

	void mark() override;
	void del() override;

	Object* add(Object* other) override;
	Object* cmp_eq(Object* other) override;
	Object* cmp_ne(Object* other) override;
	Object* cmp_lt(Object* other) override;
	Object* cmp_lte(Object* other) override;
	Object* cmp_mt(Object* other) override;
	Object* cmp_mte(Object* other) override;
	
	StrObj* repr() override;
};


struct NilObj : public Object{
	NilObj();

	void mark() override;
	void del() override;

	Object* add(Object* other) override;
	Object* cmp_eq(Object* other) override;
	Object* cmp_ne(Object* other) override;
	Object* cmp_lt(Object* other) override;
	Object* cmp_lte(Object* other) override;
	Object* cmp_mt(Object* other) override;
	Object* cmp_mte(Object* other) override;
	
	StrObj* repr() override;
};

std::ostream& operator<<(std::ostream& os, ObjKind Kind);
