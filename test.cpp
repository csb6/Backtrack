#include "catch.hpp"
#include "backtrack.hpp"

TEST_CASE("Equality checks") {
    SECTION("Atom<int>(0) == Atom<int>(0)") {
	Atom<int> a(0);
	Atom<int> b(0);
	Expression *a2 = &a;
	Expression *b2 = &b;
        REQUIRE(*a2 == *b2);
    }

    SECTION("Atom<int>(0) != Atom<long>(0)") {
	Atom<int> a(0);
	Atom<long> b(0);
	Expression *a2 = &a;
	Expression *b2 = &b;
        REQUIRE(*a2 != *b2);
    }

    SECTION("Atom<int>, different values") {
	Atom<int> a(0);
	Atom<int> b(1);
	Expression *a2 = &a;
	Expression *b2 = &b;
        REQUIRE(*a2 != *b2);
    }

    SECTION("Fact identity check") {
	Atom<int> a(0);
	Atom<int> b(1);
	Fact f({&a, &b});
	Fact f2({&a, &b});
	REQUIRE(f == f2);
    }

    SECTION("Two different Facts") {
	Atom<int> a(0);
	Atom<int> b(1);
	Fact f({&a, &b});
	Fact f2({&b, &a});
        REQUIRE(f != f2);
    }

    SECTION("Rule equality") {
	Atom<int> a;
	Atom<int> b;
	Atom<int> c(0);
	Atom<int> d(56);
	Fact f({&c, &d});

	Rule r{&a, &b};
	r << &f << &f;
	Rule r2{&a, &b};
	r2 << &f << &f;
        REQUIRE(r == r2);
    }

    SECTION("Rule inequality") {
	Atom<int> a;
	Atom<int> b;
	Atom<int> c(0);
	Atom<int> d(56);
	Fact f({&c, &d});

	Rule r{&a, &b};
	r << &f << &f;
	Rule r2{&a};
	r2 << &f << &f;
        REQUIRE(r != r2);
    }

    SECTION("Rule equality, different arities") {
	Atom<int> a;
	Atom<int> b;
	Atom<int> c(0);
	Atom<int> d(56);
	Fact f({&c, &d});

	Rule r{&a, &b};
	r << &f << &f;
	Rule r2{&a, &b};
	r2 << &f;
        REQUIRE(r != r2);
    }

    SECTION("Fact takes input") {
	Atom<int> a(1);
	Atom<int> b;
	Atom<int> c(3);

	Atom<int> a1(1);
	Atom<int> b1(2);
	Atom<int> c1(3);
	Fact add({&a1, &b1, &c1});
	std::vector<Expression*> input{&a, &b, &c};
        add(input);
	REQUIRE(b.is_filled());
	REQUIRE(b.value() == 2);
    }
}


TEST_CASE("Database checks") {
    SECTION("Construct facts using database") {
	Database<std::string> db;
	Fact &f1 = db.add_fact("add", 1, 2, 3);
	Fact &f2 = db.add_fact("add", Variable<int>(), 2, Variable<int>());
        REQUIRE(f1 == f2);
    }
}
