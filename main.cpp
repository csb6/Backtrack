/* TODO;
 - Generalize Facts as predicates or make Facts a subclass of
   Predicate, or both subclasses of abstract class??
*/
#include <iostream>
#include <vector>
#include <map>
#include <type_traits> // for std::is_same
#include <cstdint> // for std::int_least16_t
#include <array>

template<typename N, typename A, typename B>
class Truth {
public:
    virtual N name() = 0;
    virtual bool trueGiven(A a, B b) = 0;
    virtual ~Truth() {};
};

template<typename N, typename A, typename B>
class Rule : public Truth<N,A,B> {
private:
    N m_name;
    bool (*predicate)(A,B);
public:
    Rule(const N name, bool (*pred)(A,B))
	: m_name(name), predicate(pred)
    {}

    virtual N name() override
    {
	return m_name;
    }

    virtual bool trueGiven(A a, B b) override
    {
	return predicate(a,b);
    }
};

template<typename N, typename A, typename B>
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
};


template<typename N, typename A, typename B>
class LogicMachine {
    static_assert(!std::is_same<N,A>::value, "Name type must differ from A type");
    static_assert(!std::is_same<N,B>::value, "Name type must differ from B type");
private:
    using Truth_t = Truth<N,A,B>;
    using Fact_t = Fact<N,A,B>;
    using Rule_t = Rule<N,A,B>;
    std::map<N,std::vector<Truth_t*>> m_truths;
    unsigned int m_candidate_index = 0;

    Truth_t* getCandidates(const N truthName)
    {
	if(m_truths.find(truthName) == m_truths.end()) {
	    // If key doesn't exist, fact can't be found
	    return nullptr;
	}
	for(auto i=m_candidate_index; i<m_truths.size(); ++i) {
	    Truth_t* truth = m_truths[truthName][i];
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
    
    void add(const N factName, const A aValue, const B bValue)
    {
	m_truths[factName].push_back(new Fact_t(factName, aValue, bValue));
    }

    void add(const N ruleName, bool(*pred)(A,B))
    {
	m_truths[ruleName].push_back(new Rule_t(ruleName, pred));
    }

    bool isTrue(const N truthName, const A a, const B b)
    {
	Truth_t* truth = nullptr;
	while((truth = getCandidates(truthName)) != nullptr) {
	    if(truth->trueGiven(a, b)) {
		return true;
	    }
	}
	return false;
    }

    /*bool bExistsForA(const N truthName, const A a)
    {
	Truth_t* truth = nullptr;
	while((truth = getCandidates(truthName)) != nullptr) {
	    if(truth->trueGiven(a)) {
		return true;
	    }
	}
	return false;
	}*/
    
};


enum class FactName : std::int_least16_t {
    Orbits,
    InSolarSystem
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

int main()
{
    LogicMachine<FactName, Planet, Star> db;

    //Intiial Facts
    db.add(FactName::Orbits, Planet::Mercury, Star::Sun);
    db.add(FactName::Orbits, Planet::Venus, Star::Sun);
    db.add(FactName::Orbits, Planet::Earth, Star::Sun);

    std::cout << db.isTrue(FactName::Orbits, Planet::Earth, Star::AlphaCentauri) << '\n';

    return 0;
}
