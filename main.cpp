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
        Atom<int> a(1);
	Atom<int> b;
	Atom<int> c(3);

	Atom<int> a1(1);
	Atom<int> b1(2);
	Atom<int> c1(3);
	Fact add{&a1, &b1, &c1};
	std::vector<Expression*> input{&a, &b, &c};
        add(input);
	if(b.is_filled())
	    std::cout << b.value() << '\n';
	else
	    std::cout << "false\n";
    }

    return 0;
}
