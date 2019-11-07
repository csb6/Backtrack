#include "catch.hpp"
#include "backtrack.hpp"

enum class FactName {
    A, B, C, D
};

TEST_CASE("db.isTrue()") {
    Database<FactName,int,std::string> db;
    db.add(FactName::C, new Fact<int,std::string>(7));
    db.add(FactName::D, new Fact<int,std::string>("yoyoyo"));
    db.add(FactName::A, new Fact<int,std::string>(1, "Hey there"));
    db.add(FactName::A, new Fact<int,std::string>(1, "Boo yeah!"));
    db.add(FactName::A, new Fact<int,std::string>(2, "Hey there!"));
    db.add(FactName::B, new Fact<int,std::string>("hey", "how", "are", "you", 78,
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

TEST_CASE("db.deduceA()") {
    Database<FactName,int,std::string> db;
    db.add(FactName::C, new Fact<int,std::string>(7));
    db.add(FactName::D, new Fact<int,std::string>("yoyoyo"));
    db.add(FactName::A, new Fact<int,std::string>(1, "Hey there"));
    db.add(FactName::A, new Fact<int,std::string>(1, "Boo yeah!"));
    db.add(FactName::A, new Fact<int,std::string>(2, "Hey there!"));
    db.add(FactName::B, new Fact<int,std::string>("hey", "how", "are", "you", 78,
						  "doing", 89, 67, "my", "number",
						  8, "amigo", 45, 67, "", 8));

    SECTION("AB, deduce A at index 0") {
	auto val = db.deduceA(FactName::A, 0, "Hey there");
	REQUIRE(val.has_value());
	REQUIRE(val == 1);
	REQUIRE(!db.deduceA(FactName::A, 0, ""));
    }

    SECTION("BBBBABAABBABAABA, deduce A") {
	auto val = db.deduceA(FactName::B, 4, "hey", "how", "are", "you", "doing", 89, 67,
			      "my", "number", 8, "amigo", 45, 67, "", 8);
	REQUIRE(val.has_value());
	REQUIRE(val == 78);
	REQUIRE(!db.deduceA(FactName::B, 4, "hey", "how", "are", "you", "doing", 89, 67,
			    "my", "number", 8, "amigo", 45, 67, "", 9).has_value());
	REQUIRE(!db.deduceA(FactName::B, 4, "hey", "how", "are", "you", "doing", 89, 67,
			    "my", "number", 8, "amigo", 45, 67, "").has_value());
	REQUIRE(!db.deduceA(FactName::B, 4, "hey", "how", "are", "you", "doing").has_value());
    }
}
