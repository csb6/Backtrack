#ifndef BACKTRACK_LIB_H
#define BACKTRACK_LIB_H
/* Author: Cole Blakley
   File: backtrack.hpp
   Purpose: The program is a sort of Prolog within C++. You create a Database
     with three type parameters: The type for the names of facts and
     rules (usually an enum class or string), and two other types, A and
     B, representing the two types of data that can be logically-related within
     facts/rules. To get started, instantiate a Database<> with
     these 3 types, in order, parameterized in the type.

     Facts are in form `Fact<A,B>(arg0, arg1, ..., argn)`, where arg0 through
     argn are of type A and/or type B. Facts represent a concrete association
     between 2 or more values. To add a Fact to a Database instance, use
     `db.add(factName, new Fact<A,B>(arg0, arg1, ..., argn))`.
*/
#include <vector>
#include <unordered_map>
#include <type_traits> // for std::is_same
#include <functional>
#include <string>
#include <optional>
#include <any>
#include <variant>
#include <utility> // for std::forward
#include <exception>

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
    };*/

template<typename A, typename B>
class Fact {
private:
    using len_t = std::string::size_type;
    std::vector<std::variant<A,B>> m_values;
public:
    explicit Fact(A value) { m_values.insert(m_values.begin(), {value}); }
    explicit Fact(B value) { m_values.insert(m_values.begin(), {value}); }

    template<typename ...Args>
    explicit Fact(A value, Args... rest) : Fact<A,B>(rest...)
    {
	m_values.insert(m_values.begin(), {value});
    }

    template<typename ...Args>
    explicit Fact(B value, Args... rest) : Fact<A,B>(rest...)
    {
	m_values.insert(m_values.begin(), {value});
    }

    template<typename ...Args>
    bool matches(Args... rest)
    {
	return _matches(0, rest...);
    }

    template<typename Missing, typename ...Args>
    std::optional<Missing> deduce(len_t missingPos, Args... rest)
    {
	return _deduce<Missing>(missingPos, 0, rest...);
    }
private:
    template<typename T, typename ...Args>
    bool _matches(len_t pos, T value, Args... rest)
    {
        return (pos < m_values.size())
	    && std::holds_alternative<T>(m_values[pos])
	    && std::get<T>(m_values[pos]) == value
	    && _matches(pos+1, rest...);
    }
    template<typename T>
    bool _matches(len_t pos, T value)
    {
	return (pos < m_values.size())
	    && std::holds_alternative<T>(m_values[pos])
	    && std::get<T>(m_values[pos]) == value
	    && pos == m_values.size()-1;
    }

    template<typename Missing, typename T, typename ...Args>
    std::optional<Missing> _deduce(len_t missingPos, len_t pos, T value,
				   Args... rest)
    {
	if(missingPos == pos && _matches(pos+1, value, rest...)
	   && std::holds_alternative<Missing>(m_values[missingPos])) {
	    return {std::get<Missing>(m_values[pos])};
        } else if(pos < missingPos && (pos < m_values.size())
		  && std::holds_alternative<T>(m_values[pos])
		  && std::get<T>(m_values[pos]) == value) {
	    return _deduce<Missing>(missingPos, pos+1, rest...);
	} else {
	    return {};
	}
    }
    template<typename Missing, typename T>
    std::optional<Missing> _deduce(len_t missingPos, len_t pos, T value)
    {
	if(missingPos == pos
	   && _matches(pos+1, value)
	   && std::holds_alternative<Missing>(m_values[missingPos])) {
	    return {std::get<Missing>(m_values[pos])};
	} else {
	    return _deduce<Missing>(missingPos, pos+1);
	}
    }
    template<typename Missing>
    std::optional<Missing> _deduce(len_t missingPos, len_t pos)
    {
	if(missingPos == pos
	   && std::holds_alternative<Missing>(m_values[missingPos])) {
	    return {std::get<Missing>(m_values[pos])};
	} else {
	    return {};
	}
    }
};


template<typename A, typename B>
class Rule {
private:
    using len_t = std::string::size_type;
    std::vector<bool> m_args; // 1 means type A, 0 means type B
    std::any m_func;
    bool m_is_init = false;
public:
    template<typename ...Args>
    void init(bool(*func)(Args...))
    {
	(add_arg<Args>(), ...);
	// All rules MUST be init-ed before use
	// Can't use constructors because ...Args can't be inferred
	// in the templated constructor of a template class
	m_func = func;
	m_is_init = true;
    }

    template<typename ...Args>
    bool matches(Args... rest)
    {
	if(m_is_init) {
	    return _matches(0, rest...);
	} else {
	    throw std::runtime_error("Error: did not init Rule");
	}
    }

    template<typename ...Args>
    bool operator()(Args... inputs)
    {
	if(matches(inputs...)) {
	    auto func(std::any_cast<bool(*)(Args...)>(m_func));
	    return func(inputs...);
	} else {
	    return false;
	}
    }
private:
    template<typename T>
    void add_arg()
    {
	static_assert(std::is_same<T, A>::value || std::is_same<T, B>::value,
		      "Types within Rules must be of type A or type B");
        constexpr bool type = std::is_same<T, A>::value;
	m_args.push_back(type);
    }

    template<typename T, typename ...Args>
    bool _matches(len_t pos, T, Args... rest)
    {
	static_assert(std::is_same<T, A>::value || std::is_same<T, B>::value,
		      "Types within Rules must be of type A or type B");
	constexpr bool type = std::is_same<T, A>::value;
        return (pos < m_args.size())
	    && m_args[pos] == type
	    && _matches(pos+1, rest...);
    }
    template<typename T>
    bool _matches(len_t pos, T)
    {
	static_assert(std::is_same<T, A>::value || std::is_same<T, B>::value,
		      "Types within Rules must be of type A or type B");
	constexpr bool type = std::is_same<T, A>::value;
	return (pos < m_args.size())
	    && m_args[pos] == type
	    && pos == m_args.size()-1;
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
    std::unordered_map<N,std::vector<Fact<A,B>*>> m_facts;
    std::unordered_map<N,std::vector<Rule<A,B>*>> m_rules;

    Fact<A,B>* getCandidates(unsigned int index, const N truthName)
    {
	if(m_facts.find(truthName) == m_facts.end()
	   || index >= m_facts[truthName].size()) {
	    // If key doesn't exist, fact can't be found
	    return nullptr;
	}
	return m_facts[truthName][index];
    }
    Rule<A,B>* getRuleCandidates(unsigned int index, const N ruleName)
    {
	if(m_rules.find(ruleName) == m_rules.end()
	   || index >= m_rules[ruleName].size()) {
	    // If key doesn't exist, fact can't be found
	    return nullptr;
	}
	return m_facts[ruleName][index];
    }
public:
    ~Database()
    {
	for(auto &pair : m_facts) {
	    for(auto *truth : pair.second) {
		delete truth;
	    }
	}
	for(auto &pair : m_rules) {
	    for(auto *rule : pair.second) {
		delete rule;
	    }
	}
    }

    void add(const N factName, Fact<A,B> *fact)
    {
	m_facts[factName].push_back(fact);
    }

    /*void add(const N ruleName, Rule<A,B> *pred)
    {
	m_facts[ruleName].push_back(pred);
	}*/

    template<typename ...Args>
    bool operator()(const N truthName, Args... rest)
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

    template<typename T, typename ...Args>
    std::optional<T> deduce(const N truthName, len_t pos, Args... rest)
    {
	static_assert(std::is_same<T,A>::value || std::is_same<T,B>::value,
		      "Type being deduced must be same as type A or B");
	Fact<A,B> *fact = nullptr;
	unsigned int index = 0;
	std::optional<T> result;
	while((fact = getCandidates(index, truthName)) != nullptr) {
	    result = fact->template deduce<T>(pos, rest...);
	    if(result.has_value()) {
		return result;
	    }
	    ++index;
	}
	return result;
    }
};
#endif
