/*
 * A Parser object parses and validates a file into sections, keys, and values.
 */

#ifndef CONFIG_PARSER_H_
#define CONFIG_PARSER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "item.h"

namespace config {

// For readability
using std::string;
using std::unordered_map;
using std::vector;

// Helper struct that contains all information required for a single setting.
struct SingleSetting {
	public:
		string section;
		string key;
		string override;
		string value;
};

// Main parser class.
class Parser {
	public:
		// Parse a file's contents into a vector of strings for each line.
		vector<string> ParseFile(string);

		// Replace a bad character with a good one, expressed as UTF escaped strings.
		// Ref: http://stackoverflow.com/questions/17389487/c-how-to-replace-unusual-quotes-in-code
		void ReplaceChar(string&, const string&, const string&);

		// Strip string of comments and spaces (except when in quotes).
		string StripLine(string);

		// Check if a stipped line is a section header.
		bool IsValidSection(string);

		// Extract the section string from a valid section line.
		string ParseSection(string);

		// Parse a stripped line into a SingleSetting object. This is the main
		// function responsible for parsing all of the different variations of
		// a setting: key, override, and value. For valid settings, the function
		// inherits the section that is passed in. Note that the SingleSetting
		// struct object contains all string members.
		SingleSetting ParseSetting(string, string);

		// Parse a validated value into an Item object. This function is
		// responsible for converting the value string into one of the supported
		// heterogeneous types. A config item object is the type of object that is
		// exposed to the caller/user of the config system.
		Item ConstructValueObject(string);
};

namespace constants {

const char SPACE = ' ';
const char HYPHEN = '-';
const char UNDERSCORE = '_';
const char EQUALS = '=';
const char COMMENT_DELIM = ';';
const char SECTION_START = '[';
const char SECTION_END = ']';
const char QUOTE = '\"';
const char OVERRIDE_START = '<';
const char OVERRIDE_END = '>';
const char DECIMAL = '.';
const char COMMA = ',';

} // namespace constants
} // namespace Config

#endif // CONFIG_PARSER_H_
