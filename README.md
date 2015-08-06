# config-parser

### Description

This is a configuration file parser written in C++.

### Parsing Flow

The source code should be fairly self-explanatory; descriptive comments are restricted to header files, while comments pertaining to implementation details can be found in cc files.

#### [config::Item](config/item.h)

One of the first challenges with the requirements of this project pertain to the strongly-typed nature of my language of choice. Containers must always contain, and functions must return objects of the same type. Maps do not support heterogeneous types out of the box, and there are several ways to overcome this problem: base classes with Templates<T> and using third-party options such as [Boost::any](http://www.boost.org/doc/libs/1_58_0/doc/html/any.html). For the scope of this exercise, I settled for the generic approach: find a base type that is compatible with all other required types, and build a robust API that provides both type-safety as well as minimal risk to the end-user. Since C++ does not have Java's Object type (and don't even consider using `void*`), I settled for `std::string`.

The `config::Item` class is responsible for holding a single setting and provided gated access to its value with the correct type. By design, it restricts access to the type that was assigned to it by the Parser's state machine. The unit tests found in [item_test.cc](config/item_test.cc) assert that the correct types are always accessible, and asserts that a specific exception is thrown when trying to access an invalid type. The caller is expected to `try-catch` the call.

#### [config::Parser](config/parser.h)

Going one level up with our design, the meat of all parsing is performed by `config::Parser`. This class:

* Parses a file's content into individual lines
* Strips individual lines of whitespaces (except when inside quotes) and comments
* Checks if a line is a valid section
* Extracts the section string
* Checks if a line is a valid setting
* Extracts the key, override, and value strings

Assumptions made during parsing can be found in comments placed in [parser.h](config/parser.h) and [parser.cc](config/parser.cc). A comprehensive set of unit tests were absolutely paramount for a class like this, and splitting up the entire process of parsing into isolated functions helped me achieve that goal. Using the spec file as a base, several edge cases were tested in [parser_test.cc](config/parser_test.cc).

#### [config::Handler](config/handler.h)

TODO

### How to use

Please see the [Makefile](Makefile) for details. The command-line compiler `g++` is required in order to build and test this project.

* Build:

  `make build`

* Run main:

  `make run`

* Run unit tests:

  `make test`

* Clean executables:

  `make clean`

### External Libraries

I use the lest unit testing library for C++, taken from [github.com/martinmoene/lest](https://github.com/martinmoene/lest). This framework was included as a single header file under [/common/lest.hpp](common/lest.hpp).
