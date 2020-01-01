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
#include <any>
#include <utility>
#include <initializer_list>

class Expression {
public:
    // `==` means "Can this expression unify with another?"
    virtual bool operator==(const Expression &o) const = 0;
    bool operator!=(const Expression &other) const
    {
	return !(*this == other);
    }
    // This operator means: try to unify any of these arguments
    // that you can; if you can't accept these args, return false
    virtual bool operator()(std::vector<Expression*> &args) = 0;
    // Has this Expression been unified yet?
    virtual bool is_filled() const = 0;
    // Change unification status of this Expression
    virtual void set_filled(bool val) = 0;
    virtual ~Expression() {}
};


template<typename T>
class Atom : public Expression {
private:
    T m_value;
    bool m_is_filled = true;
public:
    explicit Atom() : m_is_filled(false)  {}
    explicit Atom(T value) : m_value(value)  {}
    bool operator==(const Expression &o) const override
    {
	auto &other = (const Atom<T>&)o;
	return typeid(other) == typeid(Atom<T>)
	    && (other.m_value == m_value
		|| other.m_is_filled != m_is_filled);
    }
    bool operator()(std::vector<Expression*> &args) override
    {
	auto &arg_ref{*args[0]};
	if(args.size() != 1 || typeid(arg_ref) != typeid(Atom<T>&))
	    return false;
	auto *arg = (Atom<T>*)args[0];
	if(!arg->m_is_filled) {
	    // If arg hasn't been unified, unify it with this object's value
	    arg->m_value = m_value;
	    arg->set_filled(true);
	    return true;
	} else {
	    return arg->m_value == m_value;
	}
    }
    bool is_filled() const override { return m_is_filled; }
    void set_filled(bool val) override { m_is_filled = val; }
    T value() const { return m_value; }
};


std::vector<std::size_t> unfilled_positions(const std::vector<Expression*> &args)
{
    std::size_t i = 0;
    std::vector<std::size_t> positions;
    for(const auto *each : args) {
        if(!each->is_filled())
	    positions.push_back(i);
	++i;
    }
    return positions;
}

void restore_unfilled(const std::vector<std::size_t> &positions,
		      std::vector<Expression*> &args)
{
    for(auto pos : positions) {
	args[pos]->set_filled(false);
    }
}


class Fact : public Expression {
private:
    std::vector<Expression*> m_parts;
public:
    explicit Fact(std::initializer_list<Expression*> parts)
	: m_parts(parts)
    {}
    bool operator==(const Expression &o) const override
    {
	auto &other = (const Fact&)o;
	return typeid(other) == typeid(Fact)
	    && other.m_parts == m_parts;
    }
    bool operator()(std::vector<Expression*> &args) override
    {
	if(args.size() != m_parts.size())
	    return false;
	
	std::vector<std::size_t> unfilled = unfilled_positions(args);
        for(std::size_t i = 0; i < m_parts.size(); ++i) {
	    std::vector<Expression*> arg{args[i]};
	    if(!(*m_parts[i])(arg)) {
		// Undo any unifications done by the Expressions in
		// m_parts
		restore_unfilled(unfilled, args);
		return false;
	    }
	}
	return true;
    }
    bool is_filled() const override { return true; }
    void set_filled(bool) override {}
};


class Rule : public Expression {
private:
    std::vector<Expression*> m_args;
    std::vector<Expression*> m_predicates;
public:
    explicit Rule(std::initializer_list<Expression*> args) : m_args(args)
    {}
    bool operator==(const Expression &other) const override
    {
	return typeid(other) == typeid(Rule)
	    && ((const Rule&)other).m_args == m_args
	    && ((const Rule&)other).m_predicates == m_predicates;
    }
    bool operator()(std::vector<Expression*> &args) override
    {
	if(args.size() != m_args.size())
	    return false;
        
	return false;
    }
    /*Define the series of Facts making up this Rule*/
    Rule& operator<<(Fact *part)
    {
	m_predicates.push_back((Expression*)part);
	return *this;
    }
    bool is_filled() const override { return true; }
    void set_filled(bool) override {}
};


template<typename T>
class Database {
private:
    std::map<T,std::vector<Expression*>> m_expressions;
public:
    void add(T name, Fact *new_fact)
    {
	m_expressions[name].push_back((Expression*)new_fact);
    }
    void add(T name, Rule *new_rule)
    {
	m_expressions[name].push_back((Rule*)new_rule);
    }
    bool query(T name, Expression *question)
    {
	if(m_expressions.find(name) == m_expressions.end()) {
	    return false;
	}
	return true;
    }
};
#endif
