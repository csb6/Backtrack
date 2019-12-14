# Backtrack

Backtrack is a single-header C++ library intended to serve as a rules engine
along the lines of Prolog. Currently, facts and rules similar to those in
Prolog can be defined, as well as queries without variables,
but support for backtracking and queries such as `predicate(X,5)` are not yet supported.

I see the irony of naming a library `backtrack` without supporting
backtracking, but my intention is to eventually support it in a way similar
to Prolog. For now, think of the name as me backtracking on the implication
that this library supports backtracking!

## Installation

The only file you need is `backtrack.hpp`. Simply `#include` it into any
source/header file.

This library is written in C++17.

## Usage

### Creating a database

The main class is `Database`. You can create as many databases as you
like, but typically using just one is most useful so that knowledge stays centralized.
Here is a possible instantiation of a database:

```
	#include "backtrack.hpp"

	Database<const char*, int, float> db;
```

The first parameterized type, `N`, is the type used to represent the names of
the facts and rules. Strings work great for creating names quickly, but
using an enum class called `FactName` or something similar is recommended
to minimize memory usage and reduce the range of possible fact/rule names.

The last two parameterized types represent the possible types, `A` and
`B`, of any arguments used in facts and rules.

**Note**: While `A` and `B` cannot be the same type, and `A` and `B` must be
a different type than `N`. This enforces a clear separation between what is
a fact/rule name and what is an argument.

Facts and rules can have as many overloads as you like, just like in Prolog
you can have `nameA(A).`, `nameA(A,B).`, and `name(5,6).` all in the same database.
However, all facts and rules must have at least 1 argument, meaning that you
can not define an zero-argument rule like `nameA.` in Prolog. Since C++ supports
constants and enums, it should be possible to make a `nameA` constant or enum for
the same purpose as it would be used in Prolog.

### Adding facts to the database

Facts are added by calling a member function on your database instance.
The following code adds two facts equivalent to `P(8,6)` and `Y(5)` in
Prolog.

```
	db.add("P", new Fact<int,float>(8, 6.0));
	db.add("Y", new Fact<int,float>(5.0));
```

The first argument is the name of the fact, and it must be the same type
as the `N` type specified in the `Database`'s constructor. The second argument
is a `Fact` object, which must be parameterized with the same `<A,B>` as the
`A` and `B` used in the `Database` constructor.

The arguments given to a Fact will be evaluated when added and will added immediately
to the database, so be sure that the arguments fed to the constructor are the values you
would like the Fact to use. You can specify as many arguments as like, as long as they
are all of type `A` or `B`.

Additionally, type-checking is enforced through use of templates, so the compiler
will let you know if you use arguments of types other than `A` or `B`.

## Querying the database

As seen in the last example, the `()` operator overload, `db()`, can query the
database with a given request, and the database will indicate if the query is
true or false based on its current knowledge.
Here is an example of a simple query, equivalent to the Prolog query `?- isPositive(1)`:

```
	db("isPositive", 1);
```

The first argument is the name of the rule/fact to look for; the second argument
is the first argument passed to the rule/fact. This function would return true
if there was some fact or rule named "isPositive" that evaluated to true
given an input of 1.

I hope you find the library useful and/or interesting!