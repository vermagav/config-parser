#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>

#include "parser.h"

namespace config {

// For readability
using std::string;
using std::unordered_map;
using std::vector;

vector<string> Parser::ParseFile(string filename) {
	// Try to open file
	std::ifstream file(filename);
	if (!file) {
		throw std::runtime_error(errors::FILE_OPEN);
	}

	// Read each line
	vector<string> lines;
	string line;
	while (std::getline(file, line)) {
		lines.push_back(line);
	}
	return lines;
}

void Parser::ReplaceChar(string& str, const string& from, const string& to) {
	size_t start_pos = str.find(from);
	if(start_pos == string::npos) {
		return;
	}
	str.replace(start_pos, from.length(), to);
}

string Parser::StripLine(string in) {
	string out = "";
	bool inQuotes = false;

	// Replace weird quotes with standard UTF-8 versions
	this->ReplaceChar(in, "\u201C","\u0022"); // replaces “
	this->ReplaceChar(in, "\u201D","\u0022"); // replaces ”

	for (const auto& c : in) {
	// If we encounter comment delimiter, skip rest of line
	// (but only if we're not inside quotes)
	if (c == constants::COMMENT_DELIM && !inQuotes) {
		break;
	}
	// Did we enter/leave a quote block?
	// Toggle flag, and normalize quote.
	if (c == constants::QUOTE) {
		inQuotes = !inQuotes;
		out += constants::QUOTE;
		continue;
	}
	// If not in quote block, skip spaces
	if (!inQuotes) {
		if (c == constants::SPACE) {
			continue;
		}
	}
	// Add character to out string
	out += c;
	}

	return out;
}

bool Parser::IsValidSection(string in) {
	// Note: all whitespaces have been stripped
	// A valid section must be at least 3 characters long
	// Example: "[a]"
	if (in.length() < 3) {
		return false;
	}
	// First character must be '['
	if (in[0] != constants::SECTION_START) {
		return false;
	}
	// Last character must be ']'
	if (in[in.length()-1] != constants::SECTION_END) {
		return false;
	}
	// Check remaining substring for permitted characters
	for (unsigned int i = 1; i < in.length()-1; i++) {
		const char& c = in[i];
		if (!std::isalnum(c) && c != constants::HYPHEN && c != constants::UNDERSCORE) {
			return false;
		}
	}
	// If we reached here, line is a valid section
	return true;
}

string Parser::ParseSection(string in) {
	// Note: all whitespaces have been stripped
	// Lines are expected to be valid sections
	string section = in.substr(1, in.length()-2);
	return section;
}

SingleSetting Parser::ParseSetting(string in, string section) {
	// Note: all whitespaces have been stripped
	// Lines are expected to be settings, not sections

	// Prepare default, empty setting
	SingleSetting setting;
	setting.section = "";
	setting.key = "";
	setting.override = "";
	setting.value = "";

	// A valid setting line must be at least 3 characters long
	// Example: "a=b"
	if (in.length() < 3) {
		return setting;
	}

	// Split string into key and value separated by equals
	string::size_type pos = in.find(constants::EQUALS);
	if (pos == string::npos || pos == 0 || pos == in.length()-1) {
		// Either no equals found or it is the first/last character
		// Return as invalid
		return setting;
	}

	// Grab everything on the left of quals
	string left = in.substr(0, pos);

	// Search for override
	string::size_type oStart = left.find(constants::OVERRIDE_START);
	string::size_type oEnd = left.find(constants::OVERRIDE_END);
	bool foundStart = (oStart != string::npos);
	bool foundEnd = (oEnd != string::npos);
	if (foundStart != foundEnd) {
		// Reject if we found one but not the other
		return setting;
	}
	if (foundStart && (oStart == 0 || oEnd != left.length()-1)) {
		// In order for a valid override, there must be:
		// - exactly one <, not at position 0 of left
		// - exactly one >, at position length()-1 of left
		return setting;
	}
	string override = "";
	if (foundStart) {
		for (unsigned int i = oStart+1; i < oEnd; i++) {
			const char& c = left[i];
			if (!std::isalnum(c) && c != constants::HYPHEN && c != constants::UNDERSCORE) {
				// Only allow any combination of alphanum, hyphen, and underscore
				std::cout << left[i+1] << "\t";
				return setting;
			} else {
				override += c;
			}
		}
	}

	// We liberally allow all remaining string permutations for the key & value.
	// This means the string can contain additional equal signs, and special
	// characters. This is a known design decision that can be expanded
	// with additional time. A future improvement would thus be to build
	// helper functions that validate the key and value values below.

	// Populate key
	string key = "";
	if (foundStart) {
		key = in.substr(0, oStart);
	} else {
		key = in.substr(0, pos);
	}

	// Grab everything on the right of equals as value
	string value = in.substr(pos + 1);

	// If we reached here, line is a valid setting
	setting.section = section;
	setting.key = key;
	setting.override = override;
	setting.value = value;
	return setting;
}

config::Item Parser::ConstructValueObject(string in) {
	// Note: all whitespaces have been stripped except inside quotes.
	config::Item configItem;

	// An empty string should never be sent to this function, but
	// early return to avoid accessing an index that doesn't exist later.
	if (in.length() == 0) {
		configItem.SetString(in);
		return configItem;
	}

	// Early return for quoted strings. Strip quotes before storing.
	// This is by design. When callers ask for the setting, it shouldn't
	// include the quotes -- the quotes are there to demonstrate intent that
	// the type within is a string.
	if (in[0] == constants::QUOTE) {
		string strippedString = in.substr(1, in.length()-2);
		configItem.SetString(strippedString);
		return configItem;
	}

	// Check if string is a number
	bool isNum = true;
	bool foundSign = false;
	bool foundDecimal = false;
	// Is it a signed number?
	if (in[0] == constants::HYPHEN) {
		foundSign = true;
	}
	// Check if remaining numbers are only digits.
	// Only one decimal dot allowed, and no more signs allowed.
	unsigned int i = foundSign ? 1 : 0;
	for (; i < in.length(); i++) {
		const auto& c = in[i];
		if (c == constants::DECIMAL) {
			if (foundDecimal) {
				// Already found a decimal point, this is not a valid number.
				isNum = false;
				break;
			}
			foundDecimal = true;
			continue;
		}
		if (!isdigit(c)) {
			isNum = false;
			break;
		}
	}

	// Try to convert to number
	if (isNum) {
		if (foundDecimal) {
			try {
				double doubleValue = std::stod(in);
				configItem.SetDouble(doubleValue);
				return configItem;
			} catch (std::exception e) {
				throw std::runtime_error(errors::SETTING_MAX_DOUBLE);
			}
		} else {
			try {
				int64_t intValue = std::stoll(in);
				configItem.SetInteger(intValue);
				return configItem;
			} catch (std::exception e) {
				throw std::runtime_error(errors::SETTING_MAX_INTEGER);
			}
		}
	}

	// If we reached here, we have ruled out number.
	// Next up is Bool. As a design decision, in order for the config item
	// to write itself as a boolean primitive, we will support the following
	// strings as read from the raw config file. In order to avoid ambiguity,
	// "1" and "0" without quotes will always be considered numbers, and always
	// considered strings with quotes.
	unordered_map<string, bool> permittedBoolValues = {
		{"yes", true},
		{"no", false},
		{"true", true},
		{"false", false}
	};
	if (permittedBoolValues.count(in) > 0) {
		// Found a match for bool
		configItem.SetBoolean(permittedBoolValues[in]);
		return configItem;
	}

	// The only remaining supported types are string and list of strings.
	// Check if the input string is a list of comma-separated values.
	// As a design decision, we do not permit empty strings in the list.
	// Ref: http://stackoverflow.com/questions/11719538/how-to-use-stringstream-to-separate-comma-separated-strings
	string token;
	vector<string> list;
	std::istringstream ss(in);
	while(std::getline(ss, token, constants::COMMA)) {
		if (token != "") {
			list.push_back(token);
		}
	}
	if (list.size() > 1) {
		configItem.SetList(list);
		return configItem;
	}

	// If we reached here, the only remaining supported type is string.
	configItem.SetString(in);
	return configItem;
}

} // namespace config
