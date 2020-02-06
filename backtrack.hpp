#ifndef BACKTRACK_LIB_H
#define BACKTRACK_LIB_H
/* Author: Cole Blakley
   File: backtrack.hpp

   Here is what I need to do:
   - I need to have the concept of Variables
     [X] Variables can hold any type, but only one type at a time
     [X] Variables can be compared with any other Variable
     [X] Variables can lack a value (and they usually will) or they can
         hold a value, but not both
     [ ] Variables that lack a value can contain restrictions on what their
         value can be (TODO: technically true, but need to make this usable)
     [ ] Two Variables are equal if they hold the same type, if their value-
         holding status is the same, and, if they both hold a value, that those
	 values are equivalent. If they both don't hold a value, they are equal
	 if both of their restraints are equal.
     [X] Two Variables can be unified if they hold the same type and if either: (1)
         Variable A isn't holding a value, Variable B is, and Variable B's value
	 fits the constraints of Variable A, or (2) Variable A
	 is holding a value and it's equivalent to the value held by Variable B
	 (but not both (1) and (2))
     [ ] The backtracking works by starting with the root Rule chosen based on the
         user query and recursively trying to unify the arguments given to each Rule
	 with the appropriate parameters of the predicates for each Rule until all
	 predicates for the root Rule have been proven true
   - I need to have the concept of Rules
     [X] Rules have parameters, which are inputs
       [X] Parameters are not bound to any particular thing, but instead
           contain a type and sometimes restrictions of the value they can unify with
     [ ] Rules have predicates, which are a list of names representing other Rules
         and the arguments to be passed to those Rules when the appropriate overload
	 is found. All of its predicates must be true for the Rule to be true.
   - I need to have the concept of a database
     [ ] Databases contain Rules can contain Rules with the same name as long
         as each overload takes different arguments (difference = type and/or
	 has/doesn't have a value and/or what that value is).
     [ ] The Rules present within the Database are accessible and callable from all
         of the Rules themselves.
     [ ] The user enters the name and args for a Rule, and the database tries to
         find a Rule that matches them; if one can be found, it tries to prove its
	 predicates are all true. If they fail, repeat step 1 (try to find Rule)
	 until no more unexplored overloads. This is a recursive process for
	 each of each Rule's predicates.

    - Problems:
      [ ] How to track the state of the backtracking process without mutating the
          Rules themselves
      [ ] How to unify Variables without mutating them
      [ ] How to help the user figure out what failed to unify and why
      [X] How to wire-up params with predicates of the Rules (maybe use lambdas?)
*/
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <type_traits>

class IVariable {
public:
    virtual bool can_unify(const IVariable &o) const = 0;
    virtual ~IVariable() {}
};

template<typename T>
class Variable : public IVariable {
public:
    using Predicate = bool(*)(const T&);
private:
    T m_value;
    bool m_has_value;
    std::vector<Predicate> m_constraints;
public:
    Variable() : m_has_value(false) {}

    Variable(T value) : m_value(value), m_has_value(true)
    {}

    ~Variable() {}

    virtual bool can_unify(const IVariable &o) const override
    {
	if(typeid(*this) != typeid(o))
	    // No way to unify if underlying types differ
	    return false;
	const auto &other = static_cast<const Variable<T>&>(o);

	if(is_unified() && !other.is_unified()) {
	    // This Variable has a value; can other accept that value?
	    for(const auto &predicate : other.m_constraints) {
		if(!predicate(m_value))
		    return false;
	    }
	    return true;
	} else if(!is_unified() && other.is_unified()) {
	    // Other has a value; can this Variable accept that value?
	    for(const auto &predicate : m_constraints) {
		if(!predicate(other.m_value))
		    return false;
	    }
	    return true;
	} else {
	    // If both have values, are they equivalent?
	    return is_unified() && other.is_unified()
		&& m_value == other.m_value;
	}
	return true;
    }

    bool is_unified() const { return m_has_value; }

    bool constrain(Predicate constraint)
    {
	if(m_has_value)
	    return false;
	m_constraints.push_back(constraint);
	return true;
    }

    bool set_value(T new_value)
    {
	for(const auto &predicate : m_constraints) {
	    if(!predicate(new_value))
		return false;
	}
	m_value = new_value;
	m_has_value = true;
	return true;
    }
};


template<typename T>
struct Type {};

// Holds params, but no predicates
class RuleVariable {
private:
    std::string m_name;
    std::vector<std::unique_ptr<IVariable>> m_params;

    template<typename T>
    void add_param(Type<T>)
    {
	m_params.emplace_back(new Variable<T>());
    }

    template<typename T>
    void add_param(T new_param)
    {
	m_params.emplace_back(new Variable<T>(new_param));
    }
public:
    template<typename ...Params>
    RuleVariable(std::string name, Params... params)
	: m_name(name)
    {
	(add_param(params), ...);
    }

    bool can_unify(const class Rule &other) const;

    const auto& name() const { return m_name; }

    const IVariable* operator[](std::size_t index) const
    {
        return m_params.at(index).get();
    }

    bool operator==(const RuleVariable &other) const
    {
	// Note: checks for same addresses of params, not same values
	return m_name == other.m_name && m_params == other.m_params;
    }
};


class Rule : public RuleVariable {
private:
    std::vector<RuleVariable> m_predicates;
public:
    using RuleVariable::RuleVariable;
    
    bool can_unify(const Rule &) const { return false; }

    const auto& predicates() const { return m_predicates; }

    auto& operator<<(RuleVariable &&predicate)
    {
	m_predicates.push_back(std::move(predicate));
	return *this;
    }
};


bool RuleVariable::can_unify(const Rule &other) const
{
    if(m_name != other.m_name || m_params.size() != other.m_params.size()) {
	return false;
    }

    for(std::size_t i = 0; i < m_params.size(); ++i) {
	if(!m_params[i]->can_unify(*(other.m_params[i]))) {
	    return false;
	}
    }
    return true;
}


class Database {
private:
    std::map<std::string, std::vector<Rule*>> m_rules;
public:
    void add_rule(Rule &new_rule)
    {
	m_rules[new_rule.name()].push_back(&new_rule);
    }
    
    bool query(const RuleVariable &conjecture)
    {
	if(m_rules.count(conjecture.name()) < 1)
	    return false;
	const auto &rule_vec = m_rules[conjecture.name()];
	for(const auto *rule : rule_vec) {
	    if(conjecture.can_unify(*rule)) {
	        for(const auto &each : rule->predicates()) {
                    if(!query(each))
                        return false;
                }
                return true;
	    }
	}
	return false;
    }

    template<typename ...Args>
    bool query(std::string name, Args... args)
    {
        return query(RuleVariable{name, args...});
    }
};
#endif
