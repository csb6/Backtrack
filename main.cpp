/* TODO;
 - Generalize Facts as predicates or make Facts a subclass of
   Predicate, or both subclasses of abstract class??
*/
#include <iostream>
#include <vector>
#include <map>
#include <type_traits> // for std::is_same
#include <cstdint> // for std::int_least16_t
#include <cstdarg> // for ellipsis
#include <functional>

template<typename N, typename A, typename B>
class Truth {
public:
    virtual N name() = 0;
    virtual bool trueGiven(A a, B b) = 0;
    virtual bool matches(const std::string decoder, ...);
    virtual ~Truth() {};
};

class Predicate {
private:
    std::string m_decoder;
    bool(*m_pred)(std::string,va_list);
public:
    Predicate(std::string decoder, bool(*pred)(std::string,va_list))
	: m_decoder(decoder), m_pred(pred)
    {}
    bool operator()(std::string decoder, va_list args)
    {
	if(decoder != m_decoder) {
	    return false;   
	}
        return m_pred(decoder, args);
    }
};

template<typename N, typename A, typename B>
class Rule {
private:
    N m_name;
    Predicate m_pred;
public:
    Rule(const N name, Predicate pred)
	: m_name(name), m_pred(pred)
    {}

    N name()
    {
	return m_name;
    }

    bool matches(const std::string decoder, va_list args)
    {
	return m_pred(decoder, args);
    }
};

/*template<typename N, typename A, typename B>
class Fact : public Truth<N,A,B> {
private:
    N m_name;
    A m_a;
    B m_b;
public:
    Fact(const N name, const A a, const B b)
	: m_name(name), m_a(a), m_b(b)
    {}

    virtual N name() override
    {
	return m_name;
    }

    virtual bool trueGiven(A a, B b) override
    {
	return m_a == a && m_b == b;
    }
    };*/


template<typename N, typename A, typename B>
class LogicMachine {
    static_assert(!std::is_same<N,A>::value, "Name type must differ from A type");
    static_assert(!std::is_same<N,B>::value, "Name type must differ from B type");
private:
    using Rule_t = Rule<N,A,B>;
    std::map<N,std::vector<Rule_t*>> m_truths;
    unsigned int m_candidate_index = 0;

    Rule_t* getCandidates(const N truthName)
    {
	if(m_truths.find(truthName) == m_truths.end()) {
	    // If key doesn't exist, fact can't be found
	    return nullptr;
	}
	for(auto i=m_candidate_index; i<m_truths.size(); ++i) {
	    Rule_t* truth = m_truths[truthName][i];
	    if(truth != nullptr && truth->name() == truthName) {
		m_candidate_index = i + 1;
		return truth;
	    }
	}
	return nullptr;
    }
public:
    ~LogicMachine()
    {
	for(auto &pair : m_truths) {
	    for(auto *truth : pair.second) {
		delete truth;
	    }
	}
    }
    
    /*void add(const N factName, const A aValue, const B bValue)
    {
	m_truths[factName].push_back(new Fact_t(factName, aValue, bValue));
	}*/

    void add(const N ruleName, Predicate pred)
    {
	m_truths[ruleName].push_back(new Rule_t(ruleName, pred));
    }

    bool isTrue(const N truthName, std::string decoder, ...)
    {
	va_list args;
	va_start(args, decoder);
	Rule_t* truth = nullptr;
	while((truth = getCandidates(truthName)) != nullptr) {
	    if(truth->matches(decoder, args)) {
		va_end(args);
		return true;
	    }
	}
	va_end(args);
	return false;
    }
};


enum class FactName : std::int_least16_t {
    Orbits,
    InSolarSystem,
    Truth
};

enum class Planet : std::int_least16_t {
    Mercury,
    Venus,
    Earth,
    Mars
};

enum class Star : std::int_least16_t {
    Sun,
    AlphaCentauri
};

bool alwaysTrue(std::string decoder, va_list args)
{
    Planet p = Planet::Mars;
    Star s = Star::AlphaCentauri;
    for(char argType : decoder) {
	switch(argType)
	{
	case 'a': {
	    p = va_arg(args, Planet);
	    break;
	}
	case 'b': {
	    s = va_arg(args, Star);
	    break;
	}
	default:
	    assert(1);
	    return false;
	}
    }
    return p == Planet::Mercury && s == Star::Sun;
}

int main()
{
    LogicMachine<FactName, Planet, Star> db;

    //Intiial Facts
    //db.add(FactName::Orbits, Planet::Mercury, Star::Sun);
    //db.add(FactName::Orbits, Planet::Venus, Star::Sun);
    //db.add(FactName::Orbits, Planet::Earth, Star::Sun);

    //std::cout << db.isTrue(FactName::Orbits, Planet::Earth, Star::AlphaCentauri) << '\n';

    db.add(FactName::Truth, Predicate("ab", alwaysTrue));
    std::cout << db.isTrue(FactName::Truth, "aba", Planet::Mercury, Star::Sun) << '\n';
    return 0;
}
