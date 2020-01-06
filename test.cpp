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
	Rule f({&a, &b});
	Rule f2({&a, &b});
	REQUIRE(f == f2);
    }

    SECTION("Two different Facts") {
	Atom<int> a(0);
	Atom<int> b(1);
	Rule f({&a, &b});
	Rule f2({&b, &a});
        REQUIRE(f != f2);
    }

    SECTION("Rule equality") {
	Atom<int> a;
	Atom<int> b;
	Atom<int> c(0);
	Atom<int> d(56);
	Rule f({&c, &d});

	Rule r({&a, &b});
	r << f, f;
	Rule r2({&a, &b});
	r2 << f, f;
        REQUIRE(r == r2);
    }

    SECTION("Rule inequality") {
	Atom<int> a;
	Atom<int> b;
	Atom<int> c(0);
	Atom<int> d(56);
	Rule f({&c, &d});

	Rule r({&a, &b});
	r << f, f;
	Rule r2({&a});
	r2 << f, f;
        REQUIRE(r != r2);
    }

    SECTION("Rule equality, different arities") {
	Atom<int> a;
	Atom<int> b;
	Atom<int> c(0);
	Atom<int> d(56);
	Rule f({&c, &d});

	Rule r({&a, &b});
	r << f, f;
	Rule r2({&a, &b});
	r2 << f;
        REQUIRE(r != r2);
    }

    SECTION("Fact takes input") {
	Atom<int> a(1);
	Atom<int> b;
	Atom<int> c(3);

	Atom<int> a1(1);
	Atom<int> b1(2);
	Atom<int> c1(3);
	Rule add({&a1, &b1, &c1});
	std::vector<Expression*> input{&a, &b, &c};
        add.unify(input);
	REQUIRE(b.is_unified());
	REQUIRE(b.value() == 2);
    }
}


TEST_CASE("Database checks") {
    SECTION("Construct facts using database") {
	Database<std::string> db;
	Rule &f1 = db.add("add", 1, 2, 3);
	Rule &f2 = db.add("add", Variable<int>(), 2, Variable<int>());
        REQUIRE(f1 == f2);
    }

    SECTION("get()") {
	Database<std::string> db;
	db.add("moon", "deimos", "mars");
	Expression *fact = db.get("moon", 2);
	REQUIRE(fact->arity() == 2);
    }

    SECTION("get() failure, correct name, wrong arity") {
	Database<std::string> db;
	db.add("moon", "deimos", "mars");
	Expression *fact = db.get("moon", 1);
	REQUIRE(fact == nullptr);
    }
}
