#include "catch.hpp"
#include "backtrack.hpp"

enum class FactName {
    A, B, C, D
};

TEST_CASE("db.isTrue()") {
    Database<FactName,int,const char*> db;
    db.add(FactName::C, new Fact<int,const char*>(7));
    db.add(FactName::D, new Fact<int,const char*>("yoyoyo"));
    db.add(FactName::A, new Fact<int,const char*>(1, "Hey there"));
    db.add(FactName::A, new Fact<int,const char*>(1, "Boo yeah!"));
    db.add(FactName::A, new Fact<int,const char*>(2, "Hey there!"));
    db.add(FactName::B, new Fact<int,const char*>("hey", "how", "are", "you", 78,
						  "doing", 89, 67, "my", "number",
						  8, "amigo", 45, 67, "", 8));

    SECTION("Correct Match") {
	REQUIRE(db.isTrue(FactName::A, 1, "Hey there"));
	REQUIRE(db.isTrue(FactName::A, 1, "Boo yeah!"));
	REQUIRE(db.isTrue(FactName::A, 2, "Hey there!"));
	REQUIRE(db.isTrue(FactName::B,"hey", "how", "are", "you", 78, "doing", 89, 67,
			  "my", "number", 8, "amigo", 45, 67, "", 8));
	REQUIRE(db.isTrue(FactName::C, 7));
	REQUIRE(db.isTrue(FactName::D, "yoyoyo"));
    }

    SECTION("Mismatches") {
	REQUIRE(!db.isTrue(FactName::A, 1));
	REQUIRE(!db.isTrue(FactName::A, "Boo yeah!"));
	REQUIRE(!db.isTrue(FactName::B, 1, "Hey there"));
	REQUIRE(!db.isTrue(FactName::B,"hey", "how", "are", "you", 78, "doing", 89, 67,
			   "my", "number", 8, "amigo", 45, 67, ""));
	REQUIRE(!db.isTrue(FactName::B,"hey", "how", "are", "you", 78, "doing", 89, 67,
			   "my", "number", 8, "amigo", 45, 67, 8));
	REQUIRE(!db.isTrue(FactName::B, "how", "are", "you", 78, "doing", 89, 67,
			   "my", "number", 8, "amigo", 45, 67, "", 8));
	REQUIRE(!db.isTrue(FactName::A, "hey"));
	REQUIRE(!db.isTrue(FactName::C, 1));
	REQUIRE(!db.isTrue(FactName::D, "yoyoy"));
    }
}

TEST_CASE("db.deduce() for type A") {
    Database<FactName,int,const char*> db;
    db.add(FactName::C, new Fact<int,const char*>(7));
    db.add(FactName::D, new Fact<int,const char*>("yoyoyo"));
    db.add(FactName::A, new Fact<int,const char*>(1, "Hey there"));
    db.add(FactName::A, new Fact<int,const char*>(1, "Boo yeah!"));
    db.add(FactName::A, new Fact<int,const char*>(2, "Hey there!"));
    db.add(FactName::B, new Fact<int,const char*>("hey", "how", "are", "you", 78,
						  "doing", 89, 67, "my", "number",
						  8, "amigo", 45, 67, "", 8));

    SECTION("AB, deduce A at index 0") {
	auto val = db.deduce<int>(FactName::A, 0, "Hey there");
	REQUIRE(val.has_value());
	REQUIRE(val == 1);
	REQUIRE(!db.deduce<int>(FactName::A, 0, ""));
    }

    SECTION("BBBBABAABBABAABA, deduce A") {
	auto val = db.deduce<int>(FactName::B, 4, "hey", "how", "are", "you", "doing", 89, 67,
			      "my", "number", 8, "amigo", 45, 67, "", 8);
	REQUIRE(val.has_value());
	REQUIRE(val == 78);
	REQUIRE(!db.deduce<int>(FactName::B, 4, "hey", "how", "are", "you", "doing", 89, 67,
			    "my", "number", 8, "amigo", 45, 67, "", 9).has_value());
	REQUIRE(!db.deduce<int>(FactName::B, 4, "hey", "how", "are", "you", "doing", 89, 67,
			    "my", "number", 8, "amigo", 45, 67, "").has_value());
	REQUIRE(!db.deduce<int>(FactName::B, 4, "hey", "how", "are", "you", "doing").has_value());
    }
}

TEST_CASE("db.deduce() for type B") {
    Database<FactName,int,const char*> db;
    db.add(FactName::A, new Fact<int,const char*>(65, 89, "yoyoyo", "", "", 8, 9, " "));
    db.add(FactName::A, new Fact<int,const char*>(65, 89, "yoyoyo", "", "", 8, 9, ""));
    db.add(FactName::B, new Fact<int,const char*>(98, 7));

    SECTION("AABBBAAB, deduce at index 2") {
	auto val = db.deduce<const char*>(FactName::A, 2, 65, 89, "", "", 8, 9, " ");
	REQUIRE(val.has_value());
	REQUIRE(val == "yoyoyo");
	REQUIRE(!db.deduce<const char*>(FactName::A, 2, 65, 89, "", "", 8, 9));
	REQUIRE(!db.deduce<const char*>(FactName::A, 2, 65, 89, "", "", 8, 9, "nope"));
	REQUIRE(!db.deduce<const char*>(FactName::B, 2, 65, 89, "", "", 8, 9, " "));
    }
}
