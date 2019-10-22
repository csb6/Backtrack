/* TODO;
 - Generalize Facts as predicates or make Facts a subclass of
   Predicate, or both subclasses of abstract class??
*/
#include <iostream>
#include <vector>
#include <map>
#include <cstdint> // for std::int_least16_t

template<typename Name_t, typename A_t, typename B_t>
class Fact {
public:
  Name_t name;
  A_t a;
  B_t b;
  Fact(const Name_t newName, const A_t newA, const B_t newB)
    : name(newName), a(newA), b(newB)
  {}
  Fact(const Fact &other)
  {
    name = other.name;
    a = other.a;
    b = other.b;
  }
  bool bMatches(A_t someA) const
  {
    return a == someA;
  }
  bool aMatches(B_t someB) const
  {
    return b == someB;
  }
};


template<typename Name_t, typename A_t, typename B_t>
class LogicMachine {
private:
  using Fact_t = Fact<Name_t,A_t,B_t>;
  std::map<Name_t,std::vector<Fact_t>> m_facts;
  void findFact(std::vector<Fact_t> &candidates, const Name_t factName)
  {
    if(m_facts.find(factName) == m_facts.end()) {
      // If key doesn't exist, fact can't be found
      return;
    }
    for(auto &each : m_facts[factName]) {
      candidates.push_back(each);
    }
  }
public:
  LogicMachine() {}
  void addFact(const Name_t factName, const A_t aValue, const B_t bValue)
  {
    m_facts[factName].push_back(Fact_t(factName, aValue, bValue));
  }
  
  auto findA(const Name_t factName, const B_t b)
  {
    std::vector<Fact_t> candidates;
    std::vector<A_t> solutions;
    findFact(candidates, factName);

    for(auto &fact : candidates) {
      if(fact.aMatches(b)) {
	solutions.push_back(fact.a);
      }
    }
    return solutions;
  }
  
  auto findB(const Name_t factName, const A_t a)
  {
    std::vector<Fact_t> candidates;
    std::vector<B_t> solutions;
    findFact(candidates, factName);

    for(auto &fact : candidates) {
      if(fact.bMatches(a)) {
	solutions.push_back(fact.b);
      }
    }
    return solutions;
  }
};


enum class FactName : std::int_least16_t {
	Orbits,
	Fact_Max
};

enum class Planet : std::int_least16_t {
	Mercury,
	Venus,
	Earth,
	Mars,
	Planet_Max
};

enum class Star : std::int_least16_t {
        Sun
};

int main()
{
  LogicMachine<FactName,Planet,Star> db;
  db.addFact(FactName::Orbits, Planet::Mercury, Star::Sun);
  db.addFact(FactName::Orbits, Planet::Venus, Star::Sun);
  db.addFact(FactName::Orbits, Planet::Earth, Star::Sun);

  auto sols = db.findA(FactName::Orbits, Star::Sun);
  for(auto &solution : sols) {
    std::cout << static_cast<int>(solution) << '\n';
  }
  std::cout << '\n' << static_cast<int>(Planet::Mars) << '\n';
  return 0;
}
