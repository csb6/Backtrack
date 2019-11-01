# Backtrack

Backtrack is a C++ single-header library intended to serve as a rule base,
along the lines of Prolog. Currently, facts and rules similar to those in
Prolog can be defined, as well as queries providing all of their arguments,
but support for backtracking and filling in variables in terms such as
`predicate(X,5)` are also not supported.

I see the irony of naming a library `backtrack` without supporting
backtracking, but my intention is to eventually support it in a way similar
to Prolog does. For now, think of the name as me backtracking on the
implication that this library supports backtracking!

## Installation

The only file you need is `backtrack.hpp`. Simply `#include` it into any
source/header file that requires the functionality of the library.

The library is written in C++14.

## Usage

### Creating a database

The main class is called `Database`. You can create as many databases as you
like, but typically having only one is useful so that knowledge is centralized.
Here is a possible instantiation of a database:

```
	#include "backtrack.hpp"

	Database<std::string, int, int> db;
```

The first parameterized type is the type used to represent the names of
the facts and rules. Strings work great for creating names quickly, but
using an enum class called `FactName` or something similar is recommended
to minimize memory usage and simplify the range of possible fact names.

The last two parameterized types represent the types, `A` and
`B`, that are the types of any arguments used in any fact or rule.

**Note**: While `A` and `B` can be the same type, both `A` and `B` must be
a different type than the type used to represent names. This enforces a clear
separation between what is a fact/rule name and what is an argument of facts/
rules.

Facts and rules can have as many overloads as you like, just like in Prolog
you can have `nameA(A)`, `nameA(A,B)`, and `name(5,6)` all in the same database.
However, all facts and rules must have at least 1 argument, meaning that you
can not define an argument-less rule like `nameA` in Prolog.

### Adding facts to the database

Facts are added by calling a member function of your database instance.
The following code adds two facts equivalent to `P(8,6)` and `Y(5)` in
Prolog.

```
	db.add("P", new Fact<int,int>("aa", 8, 6));
	db.add("Y", new Fact<int,int>("a", 5));
```

The first argument is the name of the fact, and it must be the same type
as the name type specified in the `Database`'s constructor. The second argument
is a `Fact` object, which must be parameterized with the same `<A,B>` as the
`A` and `B` used in the constructor.

Within `Fact`'s constructor, the first argument is a decoder string, similar to
the format string of `printf()`. The decoder string must consist only of 'a' and
'b' (all lowercase) and must match the types of the following arguments, in order. For
example, when creating the `P` fact, the arguments given are two `int` values.
Since `A` is equivalent to `int`, the decoder strings "aa". "ab", "ba", and "bb"
would all be valid, since in this case both `A` and `B` represent
`int`. However, if `B` was a different type and the second argument in the `Fact`
constructor was of type `B`, the decoder string would be "ab".

The arguments given to a Fact will be evaluated when added and will be put into
the database, so be sure the arguments fed to the constructor are the value you
would like the Fact to use.

### Adding rules to the database

Facts are nice for representing simple, base concepts, such as `parent(kevin,jeff)`,
but they cannot be generalized. So to represent a relation like `grandparent(bill,jeff)`,
you'd have to write facts by hand representing all of these relations for every
person in the database.

Rules are the solution to this issue; they allow you to add an arbitrary predicate function
(usually a lambda) to the database, which evaluates to true or false based on its
arguments. Here's how to add a `Rule` to the database that represents a similar
concept as `grandparent(X,Y,Z) :- parent(X,Y), parent(Y,Z).` does in Prolog. Assume
that the constructor for Database looked like `Database<std::string,Person,Person>`,
where `Person` is a defined class or struct:

```
	db.add("grandparent", new Rule("aaa",
		[&db](const std::string decoder, va_list args) {
			    Person grandparent = va_arg(args, Person);
			    Person parent = va_arg(args, Person);
			    Person child = va_arg(args, Person);
			    return db.isTrue("parent", "aa", grandparent, parent)
			    	   && db.isTrue("parent", "aa", parent, child);
		}));
```

This Rule will evaluate to true when called if a fact or rule named "parent"
can be found that establishes `grandparent` as the parent of `parent` and if
a fact or rule named "parent" can be found that establishes `parent` as the
parent of `child`.

## Querying the database

As seen in the last example, the member function `db.isTrue()` can query the
database with a given request, and the database will indicate if the query is
true or false based on its current knowledge.
Here is an example of simply query, equivalent to the Prolog query `?- isPositive(1)`:

```
	db.isTrue("isPositive", "a", 1);
```

The first argument is the name of the rule/fact to look for; the second argument
is the decoder string for the following arguments; and 1 is the first argument
passed to the rule/fact. This function would return true if there was some fact
or rule named "isPositive" that evaluated to true given an input of 1.

I hope you find the library useful and/or interesting!