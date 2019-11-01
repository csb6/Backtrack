/*
  This file implements examples 1-4 in horne-clause-examples.txt. Its functionality
  is equivalent to the following Prolog database and queries:
    F(3).
    F(78).
    G(3).
    U(8,6).
    Y(7,6).
    B(x) :- F(x), G(x).
    W(x) :- F(x), B(x).

    ?- B(3)
      yes
    ?- B(78)
      no
    ?- B(5)
      yes
    ?- W(3)
      no
*/
#include "backtrack.hpp"
#include <iostream>

//Rules/facts can be named F, G, B, or W
enum class Name {
    F, G, B, W
};

int main()
{
    Database<Name, int, int> db;

    //Facts
    db.add(Name::F, new Fact<int,int>("a", 3));
    db.add(Name::F, new Fact<int,int>("a", 78));
    db.add(Name::G, new Fact<int,int>("a", 3));

    //Rules
    db.add(Name::B,
	   new Rule("a",
		    [&db](const std::string decoder, va_list args) {
			int a = va_arg(args, int);
			return db.isTrue(Name::F, "a", a)
			    && db.isTrue(Name::G, "a", a);
		    }));
    db.add(Name::W,
	   new Rule("a",
		    [&db](const std::string decoder, va_list args) {
			int a = va_arg(args, int);
			return db.isTrue(Name::F, "a", a)
			    && db.isTrue(Name::B, "a", a);
		    }));

    std::cout << db.isTrue(Name::B, "a", 3) << '\n'; //Should result in 1 (true)
    std::cout << db.isTrue(Name::B, "a", 78) << '\n'; //Should result in 0 (false)
    std::cout << db.isTrue(Name::B, "a", 5) << '\n'; //Should result in 0 (false)
    std::cout << db.isTrue(Name::W, "a", 3) << '\n'; //Should result in 1 (true)

    return 0;
}
