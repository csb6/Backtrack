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
    Sun, AlphaCentauri
};

template<typename... Types>
bool matches(std::tuple<Types...> a, std::tuple<Types...> b)
{
    return a == b;
}

int main()
{
    Database<FactName, Planet, Star> db;

    Fact<int,std::string> a(2, "hey bro", 5, 6, "9");
    std::cout << "Fact matches? " << a.matches(2, "hey bro", 5, 6, "9") << '\n';

    //Initial Facts
    db.add(FactName::Orbits, new Fact<Planet,Star>(Planet::Mercury, Star::Sun));
    db.add(FactName::Orbits, new Fact<Planet,Star>(Planet::Venus, Star::Sun));
    db.add(FactName::Orbits, new Fact<Planet,Star>(Planet::Earth, Star::Sun));
    db.add(FactName::Orbits, new Fact<Planet,Star>(Star::Sun, Planet::Earth));
    db.add(FactName::InSolarSystem,
	   new Fact<Planet,Star>(Planet::Venus, Planet::Mercury, Planet::Mars,
				 Planet::Earth));
    //Predicates
    /* db.add(FactName::Truth,
	   new Rule<Planet,Star>("ab",
		    [](const std::string decoder, va_list args) {
			//db is captured, so can be used to call other predicates
			Planet p = Planet::Mars;
			Star s = Star::AlphaCentauri;
			for(char argType : decoder) {
			    if(argType == 'a') {
				p = va_arg(args, Planet);
			    } else if(argType == 'b') {
				s = va_arg(args, Star);
			    } else {
				assert(1 && "argtype in decoder string isn't 'a' or 'b'");
			    }
			}
			return p == Planet::Mercury && s == Star::Sun;
			}));*/

    std::cout << db.isTrue(FactName::Orbits, Planet::Earth, Star::Sun) << '\n';
    std::cout << db.isTrue(FactName::Truth, Planet::Mercury, Star::Sun) << '\n';
    std::cout << db.isTrue(FactName::Orbits, Planet::Venus, Star::AlphaCentauri)
	      << '\n';
    std::cout << db.isTrue(FactName::Orbits, Star::Sun, Planet::Earth) << '\n';
    std::cout << db.isTrue(FactName::InSolarSystem, Planet::Venus,
			   Planet::Mercury, Planet::Mars, Planet::Earth) << '\n';
    /*//Right now, only works with deducing arg of types within a Fact
    Planet solution = db.deduceA(FactName::Orbits, "ab", 0, Star::Sun);
    std::cout << "Solution: " << (int)solution << '\n';
    Star solution2 = db.deduceB(FactName::Orbits, "ab", 1, Planet::Venus);
    std::cout << "Solution: " << (int)solution2 << '\n';*/
    return 0;
}
