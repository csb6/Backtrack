#include "backtrack.hpp"
#include <iostream>
#include <string>

int main()
{
    Database<std::string> db;
    Fact &f1 = db.add_fact("add", 1, 2, 3);
    Fact &f2 = db.add_fact("add", Variable<int>(), 2, Variable<int>());
    assert(f1 == f2);

    return 0;
}
