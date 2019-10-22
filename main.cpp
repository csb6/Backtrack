/* TODO;
 - Generalize Facts as predicates or make Facts a subclass of
   Predicate, or both subclasses of abstract class??
*/
#include <iostream>
#include <vector>
#include <map>
#include <cstdint> // for std::int_least16_t

template<typename N, typename A, typename B>
class Rule {
public:
    N name;
    bool (*predicate)(A,B);
    Rule(const N newName, bool (*pred)(A,B))
	: name(newName), predicate(pred)
	{}
    Rule(const Rule<N,A,B> &other)
    {
	name = other.name;
	predicate = other.predicate;
    }
    bool matches(A a, B b)
    {
	return predicate(a,b);
    }
};

template<typename N, typename A, typename B>
class Fact {
public:
    N name;
    A a;
    B b;
    Fact(const N newName, const A newA, const B newB)
	: name(newName), a(newA), b(newB)
	{}
    Fact(const Fact<N,A,B> &other)
    {
	name = other.name;
	a = other.a;
	b = other.b;
    }
};


template<typename N, typename A, typename B>
class LogicMachine {
private:
    using Fact_t = Fact<N,A,B>;
    using Rule_t = Rule<N,A,B>;
    std::map<N,std::vector<Fact_t>> m_facts;
    std::map<N,std::vector<Rule_t>> m_rules;

    void getCandidates(std::vector<Fact_t> &candidates, const N factName)
    {
	if(m_facts.find(factName) == m_facts.end()) {
	    // If key doesn't exist, fact can't be found
	    return;
	}
	for(auto &rule : m_facts[factName]) {
	    candidates.push_back(rule);
	}
    }
    void getCandidates(std::vector<Rule_t> &candidates, const N ruleName)
    {
	if(m_rules.find(ruleName) == m_rules.end()) {
	    // If key doesn't exist, rule can't be found
	    return;
	}
	for(auto &rule : m_rules[ruleName]) {
	    candidates.push_back(rule);
	}
    }
public:
    LogicMachine() {}
    void add(const N factName, const A aValue, const B bValue)
    {
	m_facts[factName].push_back(Fact_t(factName, aValue, bValue));
    }

    void add(const N ruleName, bool(*pred)(A,B))
    {
	m_rules[ruleName].push_back(Rule_t(ruleName, pred));
    }

    bool isTrue(const N factName, const A a, const B b)
    {
	// First, try to find a fact that confirms name (A,B) is a
	// valid relation
	{
	    std::vector<Fact_t> candidates;
	    getCandidates(candidates, factName);
	    for(auto &fact : candidates) {
		if(fact.a == a && fact.b == b) {
		    return true;
		}
	    }
	}

	// If no fact found, try to find a rule that confirms name(A,B) is
	// a valid relation
	std::vector<Rule_t> candidates;
	getCandidates(candidates, factName);
	for(auto &rule : candidates) {
	    if(rule.matches(a,b)) {
		return true;
	    }
	}
	return false;
    }
    
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
