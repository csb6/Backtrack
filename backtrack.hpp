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
#include <map>
#include <vector>
#include <typeinfo>
#include <utility>
#include <algorithm>

// An object that can be potentially unified with another object
class Expression {
public:
    // `==` means "Can this expression unify with another?"
    virtual bool operator==(const Expression &o) const = 0;
    bool operator!=(const Expression &other) const
    {
	return !(*this == other);
    }
    // Try to unify any of these arguments
    // that you can; if you can't accept these args, return false
    virtual bool unify(std::vector<Expression*> &args) = 0;
    // Change Expression back to its original state 
    virtual void reset() = 0;
    // Has this Expression been unified yet?
    virtual bool is_unified() const = 0;
    // Change unification status of this Expression
    virtual void set_unified(bool val) = 0;
    // Number of arguments that can be given
    virtual std::size_t arity() const = 0;
    virtual ~Expression() {}
};


// A placeholder representing an Atom<T> object with no value yet
template<typename T>
struct Variable {};


// A primitive value; wraps a C++ type and contains a single value
template<typename T>
class Atom : public Expression {
private:
    T m_orig_value;
    T m_value;
    bool m_is_unified;
public:
    Atom() : m_is_unified(false) {}
    explicit Atom(T value)
	: m_orig_value(value), m_value(value), m_is_unified(true)
    {}
    bool operator==(const Expression &o) const override
    {
	auto &other = (const Atom<T>&)o;
	return typeid(o) == typeid(Atom<T>&)
	    && (other.m_value == m_value
		|| other.m_is_unified != m_is_unified);
    }
    bool unify(std::vector<Expression*> &args) override
    {
	auto &arg_ref{*args[0]};
	if(args.size() != 1 || typeid(arg_ref) != typeid(Atom<T>&))
	    return false;
	auto *arg = (Atom<T>*)args[0];
	if(!arg->m_is_unified) {
	    // If arg hasn't been unified, unify it with this object's value
	    arg->m_value = m_value;
	    arg->set_unified(true);
	    return true;
	} else {
	    return arg->m_value == m_value;
	}
    }
    void reset() override
    {
	if(m_is_unified && m_value != m_orig_value) {
	    m_value = m_orig_value;
	    set_unified(false);
	}
    }
    bool is_unified() const override { return m_is_unified; }
    void set_unified(bool val) override { m_is_unified = val; }
    T value() const { return m_value; }
    std::size_t arity() const override { return 1; }
};


// A generalized Fact that can show relations between one or more Facts
// given arbitrary input arguments
class Rule : public Expression {
private:
    std::vector<Expression*> m_args;
    std::vector<Expression*> m_predicates;
public:
    explicit Rule(std::vector<Expression*> args) : m_args(args)
    {}
    bool operator==(const Expression &o) const override
    {
	if(typeid(o) != typeid(Rule))
	    return false;
	auto &other = (const Rule&)o;
	if(other.m_args.size() != m_args.size()
	   || other.m_predicates.size() != m_predicates.size())
	    return false;

	// Check if arguments can unify
	for(std::size_t i = 0; i < m_args.size(); ++i) {
	    if(*(m_args[i]) != *(other.m_args[i]))
		return false;
	}
	// Check if predicates can unify
	for(std::size_t i = 0; i < m_predicates.size(); ++i) {
	    if(*(m_predicates[i]) != *(other.m_predicates[i]))
		return false;
	}
	return true;
    }
    bool unify(std::vector<Expression*> &args) override
    {
	if(args.size() != m_args.size())
	    return false;

	// First, try to unify the arguments with the parameters
        for(std::size_t i = 0; i < m_args.size(); ++i) {
	    std::vector<Expression*> arg{args[i]};
	    if(!m_args[i]->unify(arg)) {
		// Undo any unifications done by the Expressions in
		// m_args
	        reset();
		return false;
	    }
	}
	// TODO: try to unify the arguments with the predicates' arguments
	return true;
    }
    Expression* operator[](std::size_t index)
    {
	return m_args[index];
    }
    /*Define the series of Rules making up this Rule's body*/
    Rule& operator<<(Rule &part)
    {
	m_predicates.push_back(&part);
	return *this;
    }
    Rule& operator,(Rule &part)
    {
	return (*this << part);
    }
    void reset() override
    {
	for(auto *arg : m_args)
	    arg->reset();
	for(auto *pred : m_predicates)
	    pred->reset();
    }
    bool is_unified() const override { return true; }
    void set_unified(bool) override {}
    std::size_t arity() const override { return m_args.size(); }
};


// Contains a group of named expressions; can be queried
template<typename T>
class Database {
private:
    std::map<T,std::vector<Expression*>> m_expressions;
    std::vector<Expression*> m_atoms;
    void add_arg(Expression *expr, std::vector<Expression*> &so_far)
    {
	so_far.push_back(expr);
    }
    template<typename U>
    void add_arg(U atom, std::vector<Expression*> &so_far)
    {
	auto *new_atom = new Atom<U>(atom);
	// User enters a C++ value of type U
	m_atoms.push_back(new_atom);
	so_far.push_back(new_atom);
    }
    template<typename U>
    void add_arg(Variable<U>, std::vector<Expression*> &so_far)
    {
	auto *new_atom = new Atom<U>;
	m_atoms.push_back(new_atom);
	so_far.push_back(new_atom);
    }
public:
    ~Database()
    {
	for(auto &[key, list] : m_expressions) {
	    for(auto *expr : list) {
		delete expr;
	    }
	}
	for(auto *atom : m_atoms) {
	    delete atom;
	}
    }
    template<typename ...Args>
    Rule& add(T name, Args... args)
    {
	std::vector<Expression*> args_so_far;
	(add_arg(args, args_so_far), ...);
	auto *new_rule = new Rule(args_so_far);
	m_expressions[name].push_back(new_rule);
	return *new_rule;
    }
    Expression* get(T name, std::size_t arity)
    {
	if(m_expressions.find(name) == m_expressions.end())
	    return nullptr;
	auto match = std::find_if(m_expressions[name].begin(),
				  m_expressions[name].end(),
				  [arity](const auto *expr) {
				      return expr->arity() == arity;
				  });
	if(match == m_expressions[name].end())
	    return nullptr;
	else
	    return *match;
    }
    /*bool query(T name, Expression *question)
    {
	if(m_expressions.find(name) == m_expressions.end()) {
	    return false;
	}
	return true;
	}*/
};
#endif
