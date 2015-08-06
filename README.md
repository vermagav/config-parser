# config-parser

### Description

This is an INI configuration file parser written in C++ as part of a 24 hour hack session.

### Parsing Flow

The source code should be fairly self-explanatory; descriptive comments are restricted to header files, while comments pertaining to implementation details can be found in cc files.

#### [config::Item](config/item.h)

One of the first challenges with the requirements of this project pertain to the strongly-typed nature of my language of choice. Containers must always contain, and functions must return objects of the same type. Maps do not support heterogeneous types out of the box, and there are several ways to overcome this problem: base classes with Templates<T> and using third-party options such as [Boost::any](http://www.boost.org/doc/libs/1_58_0/doc/html/any.html). For the scope of this exercise, I settled for the generic approach: find a base type that is compatible with all other required types, and build a robust API that provides both type-safety as well as minimal risk to the end-user. Since C++ does not have Java's Object type (and don't even consider using `void*`), I settled for `std::string`.

The `config::Item` class is responsible for holding a single setting and provides gated access to its value with a set type. By design, it restricts access to the type that was assigned to it by the Parser's state machine. The unit tests found in [item_test.cc](config/item_test.cc) assert that the correct types are always accessible, and asserts that a specific exception is thrown when trying to access an invalid type. The caller is expected to `try-catch` the call.

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

This is the only class that users should touch to load and use a config file. As a trade off against memory and in favor of fast access, two maps are used to fetch both individual settings as well as a map of all settings for a section in O(1) time (most of the time, not considering rare, worst cases due to `unordered_map` collisions).

As seen in [main.cc](main.cc), typical usage is as follows:

```c++
// Get a config handler
config::Handler handler;

// Load a config file
handler.Load("sample.ini", {"production", "ubuntu"});

// Output some settings to standard output
config::Item* setting;
setting = handler.Get("common.paid_users_size_limit");
std::cout << setting->GetInteger();
```

Swap out the sample ini file for different test files to see various results.

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
  
#### Example:

```text
gverma at ubuntu in ~/Code/config-parser on master
$ make run
g++ -Wall -Wno-unused-variable -std=c++11 main.cc config/handler.cc config/item.cc config/parser.cc -o bin/config_parser
./bin/config_parser


Test print directly: 2147483648
[INT]:	2147483648
[STR]:	hello there, ftp uploading
[LIST]:	{ array, of, values }
[NONE]:	Setting not found.
[BOOL]:	0
[STR]:	/etc/var/uploads


Printing ALL keys and values via GetSection():
[KEY]:	path:
[STR]:	/srv/var/tmp/
[KEY]:	paid_users_size_limit:
[INT]:	2147483648
[KEY]:	student_size_limit:
[INT]:	52428800
[KEY]:	basic_size_limit:
[INT]:	26214400

```

### External Libraries

I use the lest unit unit-test framework for C++, taken from [github.com/martinmoene/lest](https://github.com/martinmoene/lest). This framework was included as a single header file and can be found at [common/lest.hpp](common/lest.hpp).

### Future Improvements

Here are some improvements I thought about but did not have time to implement due to the 24 hour time constraint:

* Reload functionality
  - Check for changes in a file on disk (which could be updated via Zookeeper on a server)
  - Load file when changes are detected, flushing old map after successful reload
* Limit the size of the ini file
  - As of now, there is no upper bound on how large the input file should be
  - The parser will continue to load new lines in memory until it runs out of memory, at which point the process will either be terminated (depening on OOM policies on the machine), or the OS will start paging to disk, slowing down the load tremendously.
* Limit the size of each line in the ini file
  - As of now, a line could be of an unbounded length
  - Same issues as above
* Add additional validation to key and value strings
  - See [this comment](config/parser.cc#L164)
* Turn `config::Handler` into a singleton class
