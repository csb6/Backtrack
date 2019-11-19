#ifndef BACKTRACK_LIB_H
#define BACKTRACK_LIB_H
/* Author: Cole Blakley
   File: backtrack.hpp
   Purpose: The program is a sort of Prolog within C++. You create a Database
     with three type parameters: The type for the names of facts and
     predicates (usually an enum class or string), and two other types, A and
     B, representing the two types of data that can be logically-related within
     facts/predicates. To get started, instantiate a Database<> with
     these 3 types, in order, parameterized in the type.

     Facts are in form `factname(decoder, val1, val2, ...)`, where val1 and
     val2 are of type A or type B. Facts represent a concrete association
     between 2 values, a and b. To add a Fact to a Database instance, do
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
#include <optional>

/*template<typename A, typename B>
class Truth {
     static_assert(!std::is_same<A,B>::value, "A and B must be different types");
public:
    virtual bool matches(A value) = 0;
    virtual bool matches(B value) = 0;
    template<typename ...Args>
    virtual bool matches(A value, Args... rest) = 0;
    template<typename ...Args>
    virtual bool matches(B value, Args... rest) = 0;
    virtual std::optional<A> deduceA(const std::string decoder, unsigned int pos,
				     va_list args) = 0;
    virtual std::optional<B> deduceB(const std::string decoder, unsigned int pos,
				     va_list args) = 0;
    virtual ~Truth() {};
};

template<typename A, typename B>
class Rule : public Truth<A,B> {
private:
    const std::string m_decoder;
    std::function<bool(std::string,va_list)> m_pred;
public:
    Rule(const std::string decoder, std::function<bool(std::string,va_list)> pred)
	: m_decoder(decoder), m_pred(pred)
    {}

    virtual bool matches(A value) override { return false; }
    virtual bool matches(B value) override { return false; }
    
    template<typename ...Args>
    virtual bool matches(A value, Args... rest) override
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

    template<typename ...Args>
    virtual bool matches(B value, Args... rest) { return false; }

    virtual std::optional<A> deduceA(const std::string decoder, unsigned int pos,
				     va_list args) override
    {
	return {};
    }

    virtual std::optional<B> deduceB(const std::string decoder, unsigned int pos,
				     va_list args) override
    {
	return {};
    }
};*/

template<typename A, typename B>
class Fact {
private:
    std::vector<A> m_a;
    std::vector<B> m_b;
    using len_t = std::string::size_type;
public:
    std::string m_decoder;
    explicit Fact(A value) { m_a.insert(m_a.begin(), value); m_decoder = "a" + m_decoder; }
    explicit Fact(B value) { m_b.insert(m_b.begin(), value); m_decoder = "b" + m_decoder; }
    template<typename ...Args>
    explicit Fact(A value, Args... rest) : Fact<A,B>(rest...)
    {
	m_a.insert(m_a.begin(), value);
	m_decoder = "a" + m_decoder;
    }
    template<typename ...Args>
    explicit Fact(B value, Args... rest) : Fact<A,B>(rest...)
    {
	m_b.insert(m_b.begin(), value);
	m_decoder = "b" + m_decoder;
    }

    template<typename ...Args>
    bool matches(Args... rest)
    {
	return _matches(0, 0, 0, rest...);
    }
private:
    template<typename T, typename ...Args>
    bool _matches(len_t index, len_t indexA, len_t indexB, T value, Args... rest)
    {
	static_assert(std::is_convertible<T, A>::value || std::is_convertible<T, B>::value);
	if constexpr(std::is_convertible<T, A>::value) {
	    if(indexA < m_a.size() && index < m_decoder.size()
	       && m_decoder[index] == 'a' && m_a[indexA] == value) {
		return _matches(index+1, indexA+1, indexB, rest...);
	    }
	} else {
	    if(indexB < m_b.size() && index < m_decoder.size()
	       && m_decoder[index] == 'b' && m_b[indexB] == value) {
		return _matches(index+1, indexA, indexB+1, rest...);
	    }
	}
	return false;
    }

    template<typename T>
    bool _matches(len_t index, len_t indexA, len_t indexB, T value)
    {
	static_assert(std::is_convertible<T, A>::value || std::is_convertible<T, B>::value);
	if constexpr(std::is_convertible<T, A>::value) {
	    if(indexA < m_a.size() && index == m_decoder.size()-1
	       && m_a[indexA] == value) {
		return true;
	    }
	} else {
	    if(indexB < m_b.size() && index == m_decoder.size()-1
	       && m_b[indexB] == value) {
		return true;
	    }
	}
	return false;
    }
public:
    template<typename ...Args>
    std::optional<A> deduceA(len_t goal, Args... rest)
    {
	return _deduceA(0, 0, 0, goal, rest...);
    }
private:
    template<typename T>
    std::optional<A> _deduceA(len_t index, len_t indexA, len_t indexB, len_t goal,
			      T value)
    {
	static_assert(std::is_convertible<T, A>::value || std::is_convertible<T, B>::value);
	if constexpr(std::is_convertible<T, A>::value) {
	    if(indexA < m_a.size() && index != m_decoder.size()-2
	       &&index == goal && m_a[indexA+1] == value) {
		return m_a[indexA];
	    }
	} else {
	    if(indexB < m_b.size() && index == m_decoder.size()-2
	       && index == goal && m_b[indexB] == value) {
		return m_a[indexA];
	    }
	}
	return {};
    }
    template<typename T, typename ...Args>
    std::optional<A> _deduceA(len_t index, len_t indexA, len_t indexB, len_t goal,
			      T value, Args... rest)
    {
	static_assert(std::is_convertible<T, A>::value || std::is_convertible<T, B>::value);
	if constexpr(std::is_convertible<T, A>::value) {
	    if(indexA >= m_a.size() || index >= m_decoder.size()) {
		return {};
	    } else if(index == goal && _matches(index+1, indexA+1, indexB, value, rest...)) {
		return m_a[indexA];
	    } else if(index != goal && m_decoder[index] == 'a'
		      && m_a[indexA] == value) {
		return _deduceA(index+1, indexA+1, indexB, goal, rest...);
	    }
	} else {
	    if(indexB >= m_b.size() || index >= m_decoder.size()) {
		return {};
	    } else if(index == goal && _matches(index+1, indexA+1, indexB, value, rest...)) {
		return m_a[indexA];
	    } else if(index != goal && m_decoder[index] == 'b'
		      && m_b[indexB] == value) {
		return _deduceA(index+1, indexA, indexB+1, goal, rest...);
	    }
	}
	return {};
    }
};

template<typename N, typename A, typename B>
class Database {
    static_assert(!std::is_same<N,A>::value, "Name type must differ from A type");
    static_assert(!std::is_same<N,B>::value, "Name type must differ from B type");
    static_assert(!std::is_convertible<A,B>::value,
		  "A and B cannot be implicitly convertible");
    using len_t = std::string::size_type;
private:
    std::map<N,std::vector<Fact<A,B>*>> m_truths;

    Fact<A,B>* getCandidates(unsigned int index, const N truthName)
    {
	if(m_truths.find(truthName) == m_truths.end()
	   || index >= m_truths[truthName].size()) {
	    // If key doesn't exist, fact can't be found
	    return nullptr;
	}
	return m_truths[truthName][index];
    }
public:
    ~Database()
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

    /*void add(const N ruleName, Rule<A,B> *pred)
    {
	m_truths[ruleName].push_back(pred);
	}*/

    template<typename ...Args>
    bool isTrue(const N truthName, Args... rest)
    {
	Fact<A,B> *fact = nullptr;
	unsigned int index = 0;
	while((fact = getCandidates(index, truthName)) != nullptr) {
	    if(fact->matches(rest...)) {
		return true;
	    }
	    ++index;
	}
	return false;
    }

    template<typename ...Args>
    std::optional<A> deduceA(const N truthName, len_t pos, Args... rest)
    {
	Fact<A,B> *fact = nullptr;
	unsigned int index = 0;
	std::optional<A> result;
	while((fact = getCandidates(index, truthName)) != nullptr) {
	    result = fact->deduceA(pos, rest...);
	    if(result.has_value()) {
		return result;
	    }
	    ++index;
	}
	return result;
    }

    /*template<typename ...Args>
    std::optional<B> deduceB(const N truthName, unsigned int pos, Args... rest)
    {
	Fact<A,B> *fact = nullptr;
	unsigned int index = 0;
	std::optional<B> result;
	while((fact = getCandidates(index, truthName)) != nullptr) {
	    result = fact->deduceB(pos, rest...);
	    if(result.has_value()) {
		return result;
	    }
	    ++index;
	}
	return result;
	}*/
};
#endif
