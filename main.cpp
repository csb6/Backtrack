#include "backtrack.hpp"
#include <iostream>
#include <string>

int main()
{
    {
	Atom<int> a(0);
	Atom<int> b(0);
	Expression *a2 = &a;
	Expression *b2 = &b;
	assert(*a2 == *b2);
    }

    {
	Atom<int> a(0);
	Atom<long> b(0);
	Expression *a2 = &a;
	Expression *b2 = &b;
	assert(*a2 != *b2);
    }

    {
	Atom<int> a(0);
	Atom<int> b(1);
	Expression *a2 = &a;
	Expression *b2 = &b;
	assert(*a2 != *b2);
    }

    {
	Atom<int> a(0);
	Atom<int> b(1);
	Fact f{&a, &b};
	Fact f2{&a, &b};
	assert(f == f2);
    }

    {
	Atom<int> a(0);
	Atom<int> b(1);
	Fact f{&a, &b};
	Fact f2{&b, &a};
	assert(f != f2);
    }

    {
	Atom<int> a;
	Atom<int> b;
	Atom<int> c(0);
	Atom<int> d(56);
	Fact f{&c, &d};
	
	Rule r{&a, &b};
	r << &f << &f;
	Rule r2{&a, &b};
	r2 << &f << &f;
	assert(r == r2);
    }

    {
	Atom<int> a;
	Atom<int> b;
	Atom<int> c(0);
	Atom<int> d(56);
	Fact f{&c, &d};
	
	Rule r{&a, &b};
	r << &f << &f;
	Rule r2{&a};
	r2 << &f << &f;
	assert(r != r2);
    }

    {
	Atom<int> a;
	Atom<int> b;
	Atom<int> c(0);
	Atom<int> d(56);
	Fact f{&c, &d};
	
	Rule r{&a, &b};
	r << &f << &f;
	Rule r2{&a, &b};
	r2 << &f;
	assert(r != r2);
    }

    {
	Database<std::string> db;
	Atom joe("joe");
	Atom jeff("jeff");
	Fact f{&joe, &jeff};
	db.add("parent", &f);
    }

    return 0;
}
