#include "backtrack.hpp"
#include <iostream>
#include <string>

int main()
{
    Database<std::string> db;
    Rule &m1 = db.add("moon", "Moon", "Earth");
    Rule &m2 = db.add("moon", "Phobos", "Mars");
    db.add("moon", "Deimos", "Mars");
    Rule &planet = db.add("planet", Variable<std::string>());
    planet << &m1, &m2;
    
    return 0;
}
