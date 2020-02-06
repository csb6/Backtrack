#include "backtrack.hpp"
#include <iostream>
#include <cassert>

int main()
{
    {
        Variable<int> a;
        assert(!a.is_unified());
        a.constrain([](const auto &value) {
                        return value > 5;
                    });
        assert(!a.set_value(5));
        assert(a.set_value(6));


        Variable b("hello there");
        assert(b.is_unified());

        Variable<int> c;
        const Variable d(78);
        c.constrain([](const auto &value) {
                        return value != 87;
                    });
        c.constrain([](const auto &value){
                        return value > 56;
                    });
        assert(d.can_unify(c) && c.can_unify(d));
        c.constrain([](const auto &value) {
                        return value != 78;
                    });
        assert(!d.can_unify(c) && !c.can_unify(d));

        Variable e(78);
        Variable f(78);
        assert(e.can_unify(f) && f.can_unify(e));
    }

    {
        Database db;

        Rule a{Type<int>(), Type<int>(), 2};
        //Rule b{Type<int>(), 5};
        //Rule c{5, 6};
        //db.add_rule(a);
        //db.add_rule(b);
        //db.add_rule(c);
    }
    return 0;
}
