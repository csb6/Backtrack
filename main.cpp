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

int main()
{
    LogicMachine<FactName, Planet, Star> db;

    //Initial Facts
    db.add(FactName::Orbits, new Fact<Planet,Star>("ab", Planet::Mercury, Star::Sun));
    db.add(FactName::Orbits, new Fact<Planet,Star>("ab", Planet::Venus, Star::Sun));
    db.add(FactName::Orbits, new Fact<Planet,Star>("ab", Planet::Earth, Star::Sun));
    db.add(FactName::Orbits, new Fact<Planet,Star>("ba", Star::Sun, Planet::Earth));
    db.add(FactName::InSolarSystem,
	   new Fact<Planet,Star>("aaaa", Planet::Venus, Planet::Mercury, Planet::Mars,
				 Planet::Earth));
    //Predicates
    db.add(FactName::Truth,
	   new Rule("ab",
		    [&db](const std::string decoder, va_list args) {
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
		    }));

    std::cout << db.isTrue(FactName::Orbits, "ab", Planet::Earth, Star::Sun) << '\n';
    std::cout << db.isTrue(FactName::Truth, "ab", Planet::Mercury, Star::Sun) << '\n';
    std::cout << db.isTrue(FactName::Orbits, "ab", Planet::Venus, Star::AlphaCentauri)
	      << '\n';
    std::cout << db.isTrue(FactName::Orbits, "ba", Star::Sun, Planet::Earth) << '\n';
    std::cout << db.isTrue(FactName::InSolarSystem, "aaaa", Planet::Venus,
			   Planet::Mercury, Planet::Mars, Planet::Earth) << '\n';
    return 0;
}
