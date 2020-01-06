#include "backtrack.hpp"
#include <iostream>
#include <string>

int main()
{
    Database<std::string> db;
    Fact &m1 = db.add_fact("moon", "Moon", "Earth");
    Fact &m2 = db.add_fact("moon", "Phobos", "Mars");
    db.add_fact("moon", "Deimos", "Mars");
    Rule &planet = db.add_rule("planet", Variable<std::string>());
    planet << &m1, &m2;
    
    return 0;
}
