/* Author: Cole Blakley
   File: main.cpp
   Purpose: The program is a sort of Prolog within C++. You create a Logic
     Machine with three type parameters: The type for the names of facts and
     predicates (usually an enum class or string), and two other types, A and
     B, representing the two types of data that can be logically-related within
     facts/predicates. To get started, instantiate a LogicMachine<> with
     these 3 types, in order, parameterized in the type.

     Facts must (currently) be in form `factname(A a, B b)`. They represent
     a concrete association between 2 values, a and b. To add a Fact to a
     LogicMachine instance, do `lm.add(factName, new Fact<A,B>(aVal, bVal))`.

     Rules are essentially function objects that can take in variable numbers
     of arguments. Using a given function pointer that returns true or false,
     a rule will match or not match with a given query. To make a new rule,
     first define a function/lambda, and pass its name to
     ln.add(ruleName, new Rule(decoder,fcnPtr)). decoder is a string consisting
     of lowercase 'a' and 'b' representing the argument order and type that
     the rule's function ptr accepts. For example, a function that needs an
     A value and a B value would have a decoder string "ab".
*/
#include <iostream>
#include <vector>
#include <map>
#include <type_traits> // for std::is_same
#include <cstdint> // for std::int_least16_t
#include <cstdarg> // for ellipsis
#include <functional>

class Truth {
public:
    virtual bool matches(std::string decoder, va_list args) = 0;
    virtual ~Truth() {};
};

class Rule : public Truth {
private:
    std::string m_decoder;
    bool(*m_pred)(std::string,va_list);
public:
    Rule(std::string decoder, bool(*pred)(std::string,va_list))
	: m_decoder(decoder), m_pred(pred)
    {}
    virtual bool matches(std::string decoder, va_list args) override
    {
	if(decoder != m_decoder) {
	    return false;   
	}
        return m_pred(decoder, args);
    }
};

template<typename A, typename B>
class Fact : public Truth {
private:
    A m_a;
    B m_b;
public:
    Fact(A a, B b) : m_a(a), m_b(b)
    {}
    virtual bool matches(std::string decoder, va_list args) override
    {
	if(decoder != "ab") {
	    return false;  
	}
	//Have to copy args since taking items out of args is destructive
	va_list argsCopy;
	va_copy(argsCopy, args);
	A a = va_arg(argsCopy, A);
	B b = va_arg(argsCopy, B);
	va_end(argsCopy);
        return a == m_a && b == m_b;
    }
};


template<typename N, typename A, typename B>
class LogicMachine {
    static_assert(!std::is_same<N,A>::value, "Name type must differ from A type");
    static_assert(!std::is_same<N,B>::value, "Name type must differ from B type");
private:
    std::map<N,std::vector<Truth*>> m_truths;
    
    Truth* getCandidates(unsigned int index, const N truthName)
    {
	if(m_truths.find(truthName) == m_truths.end()
	   || index >= m_truths[truthName].size()) {
	    // If key doesn't exist, fact can't be found
	    return nullptr;
	}
	return m_truths[truthName][index];
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

    void add(const N factName, Fact<A,B> *fact)
    {
	m_truths[factName].push_back(fact);
    }

    void add(const N ruleName, Rule *pred)
    {
	m_truths[ruleName].push_back(pred);
    }

    bool isTrue(const N truthName, std::string decoder, ...)
    {
	va_list args;
	va_start(args, decoder);
	Truth* truth = nullptr;
	unsigned int index = 0;
	while((truth = getCandidates(index, truthName)) != nullptr) {
	    if(truth->matches(decoder, args)) {
		va_end(args);
		return true;
	    }
	    ++index;
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
    db.add(FactName::Orbits, new Fact<Planet,Star>(Planet::Mercury, Star::Sun));
    db.add(FactName::Orbits, new Fact<Planet,Star>(Planet::Venus, Star::Sun));
    db.add(FactName::Orbits, new Fact<Planet,Star>(Planet::Earth, Star::Sun));

    std::cout << db.isTrue(FactName::Orbits, "ab", Planet::Earth, Star::Sun)
	      << '\n';

    //Predicates
    db.add(FactName::Truth, new Rule("ab", alwaysTrue));
    
    std::cout << db.isTrue(FactName::Truth, "ab", Planet::Mercury, Star::Sun) << '\n';

    std::cout << db.isTrue(FactName::Orbits, "ab", Planet::Venus, Star::AlphaCentauri)
	      << '\n';
    return 0;
}
