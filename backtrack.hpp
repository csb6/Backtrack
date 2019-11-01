#ifndef BACKTRACK_LIB_H
#define BACKTRACK_LIB_H
/* Author: Cole Blakley
   File: backtrack.hpp
   Purpose: The program is a sort of Prolog within C++. You create a Logic
     Machine with three type parameters: The type for the names of facts and
     predicates (usually an enum class or string), and two other types, A and
     B, representing the two types of data that can be logically-related within
     facts/predicates. To get started, instantiate a LogicMachine<> with
     these 3 types, in order, parameterized in the type.

     Facts are in form `factname(decoder, val1, val2, ...)`, where val1 and
     val2 are of type A or type B. Facts represent a concrete association
     between 2 values, a and b. To add a Fact to a LogicMachine instance, do
     `lm.add(factName, new Fact<A,B>(aVal, bVal))`.

     Rules are essentially function objects that can take in variable numbers
     of arguments. Using a given function pointer that returns true or false,
     a rule will match or not match with a given query. To make a new rule,
     first define a function/lambda, and pass its name to
     ln.add(ruleName, new Rule(decoder,functor)). decoder is a string consisting
     of lowercase 'a' and 'b' representing the argument order and type that
     the rule's functor accepts. For example, a function that needs an
     A value and a B value would have a decoder string "ab".
*/
#include <vector>
#include <map>
#include <type_traits> // for std::is_same
#include <cstdarg> // for ellipsis
#include <functional>
#include <string>

class Truth {
public:
    virtual bool matches(const std::string decoder, va_list args) = 0;
    virtual ~Truth() {};
};

class Rule : public Truth {
private:
    const std::string m_decoder;
    std::function<bool(std::string,va_list)> m_pred;
public:
    Rule(const std::string decoder, std::function<bool(std::string,va_list)> pred)
	: m_decoder(decoder), m_pred(pred)
    {}
    virtual bool matches(const std::string decoder, va_list args) override
    {
	if(decoder != m_decoder) {
	    return false;
	}
	//Have to copy args since taking items out of args is destructive
	va_list argsCopy;
	va_copy(argsCopy, args);
        bool result = m_pred(decoder, argsCopy);
	va_end(argsCopy);
	return result;
    }
};

template<typename A, typename B>
class Fact : public Truth {
private:
    std::vector<A> m_a;
    std::vector<B> m_b;
    const std::string m_decoder;
public:
    Fact(const std::string decoder, ...) : m_decoder(decoder)
    {
	va_list args;
	va_start(args, decoder);
	for(char argType : decoder) {
	    switch(argType)
	    {
	    case 'a': {
		m_a.push_back(va_arg(args, A));
		break;
	    }
	    case 'b': {
		m_b.push_back(va_arg(args, B));
		break;
	    }
	    default:
		assert(1 && "argtype in decoder string isn't 'a' or 'b'");
	    }
	}
	va_end(args);
    }

    virtual bool matches(const std::string decoder, va_list args) override
    {
	if(decoder != m_decoder) {
	    return false;
	}
	//Have to copy args since taking items out of args is destructive
	va_list argsCopy;
	va_copy(argsCopy, args);
	//Need to keep track of where checked so index adjusted for m_a & m_b
	int aCount = 0;
	int bCount = 0;
	for(unsigned int i=0; i<decoder.size(); ++i) {
	    if(decoder[i] == 'a') {
		if(va_arg(argsCopy, A) != m_a[i-bCount]) {
		    va_end(argsCopy);
		    return false;
		}
		++aCount;
	    } else {
		if(va_arg(argsCopy, B) != m_b[i-aCount]) {
		    va_end(argsCopy);
		    return false;
		}
		++bCount;
	    }
	}
	va_end(argsCopy);
        return true;
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

    bool isTrue(const N truthName, const std::string decoder, ...)
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
#endif
