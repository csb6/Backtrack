/*TODO
[ ] Add some sort of logging mechanism so I can see which Fact/Rules are
    considered and rejected, along with each of these overloads' type signatures
[ ] Add support for some sort of lombda function for Rules so that arbitrary C++
    code can be run as part of the program
[ ] Add support for Rules within the Database, maybe in a separate collection
    from facts or maybe in the same collection, using std::variant
[ ] Add support for basic backtracking between Facts and Rules
*/
#include "backtrack.hpp"
#include <iostream>
#include <cstdint> // for std::int_least16_t

enum class FactName : std::int_least16_t {
    Orbits, InSolarSystem, Truth
};

enum class Planet {
    Mercury, Venus, Earth, Mars
};

enum class Star {
    AlphaCentauri, Sun
};

int main()
{
    Database<FactName, Planet, Star> db;

    Fact<int,const char*> a(2, "hey bro", 5, 6, "9");
    std::cout << "Fact matches? " << a.matches(2, "hey bro", 5, 6, "9") << '\n';

    //Initial Facts
    db.add(FactName::Orbits, new Fact<Planet,Star>(Planet::Mercury, Star::Sun));
    db.add(FactName::Orbits, new Fact<Planet,Star>(Planet::Venus, Star::Sun));
    db.add(FactName::Orbits, new Fact<Planet,Star>(Planet::Earth, Star::Sun));
    db.add(FactName::Orbits, new Fact<Planet,Star>(Star::Sun, Planet::Earth));
    db.add(FactName::Orbits, new Fact<Planet,Star>(Star::Sun));
    db.add(FactName::InSolarSystem,
	   new Fact<Planet,Star>(Planet::Venus, Planet::Mercury, Planet::Mars,
				 Planet::Earth));


    std::cout << db(FactName::Orbits, Planet::Earth, Star::Sun) << '\n';
    std::cout << db(FactName::Truth, Planet::Mercury, Star::Sun) << '\n';
    std::cout << db(FactName::Orbits, Planet::Venus, Star::AlphaCentauri)
	      << '\n';
    std::cout << db(FactName::Orbits, Star::Sun, Planet::Earth) << '\n';
    std::cout << db(FactName::InSolarSystem, Planet::Venus,
			   Planet::Mercury, Planet::Mars, Planet::Earth) << '\n';
    std::cout << db(FactName::Orbits, Star::Sun) << '\n';
    //Right now, only works with deducing arg of types within a Fact
    auto solution = db.deduce<Planet>(FactName::Orbits, 0, Star::Sun);
    std::cout << "Solution: " << (int)solution.value() << '\n';
    auto solution2 = db.deduce<Star>(FactName::Orbits, 1, Planet::Venus);
    std::cout << "Solution: " << (int)solution2.value() << '\n';

    Rule<Planet, Star> rule1;
    rule1.init<Planet, Star, Planet, Planet>();
    std::cout << "Rule matches? "
	      << rule1.matches(Planet::Earth, Star::Sun, Planet::Earth,
			       Planet::Mars) << '\n';

    return 0;
}
