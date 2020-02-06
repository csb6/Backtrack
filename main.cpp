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

        Rule a{"a", Type<int>(), Type<int>(), 2};
        a << RuleVariable{"b", 2, 2};

        Rule b{"b", 2, 2};
        db.add_rule(a);
        db.add_rule(b);
        std::cout << db.query(RuleVariable{"a", 45453, -890, 2}) << '\n';
        std::cout << db.query("a", 45453, -890, "oiii") << '\n';
        std::cout << db.query("a", 45453) << '\n';
    }
    return 0;
}
