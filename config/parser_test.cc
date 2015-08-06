#include <iostream>
#include <stdexcept>

#include "../common/lest.hpp"
#include "parser.h"

const lest::test specification[] = {
	CASE("StripLine correctly strips lines") {
		config::Parser parser;

		// Remove whitespaces
		EXPECT(parser.StripLine("") == "");
		EXPECT(parser.StripLine("path = /tmp/") == "path=/tmp/");
		EXPECT(parser.StripLine("path<production> = /srv/var/tmp/") == "path<production>=/srv/var/tmp/");
		EXPECT(parser.StripLine("basic_size_limit = 26214400") == "basic_size_limit=26214400");

		// ... except when whitespace is inside quotes (only double-quotes are considered quotes)
		EXPECT(parser.StripLine("name = \"hello there, ftp uploading\"") == "name=\"hello there, ftp uploading\"");
		EXPECT(parser.StripLine("name = \"http uploading\"") == "name=\"http uploading\"");

		// Handle weird, non-standard quote marks
		EXPECT(parser.StripLine("name = “hello there, ftp uploading”") == "name=\"hello there, ftp uploading\"");
		EXPECT(parser.StripLine("name = “http uploading”") == "name=\"http uploading\"");

		// Remove comments
		EXPECT(parser.StripLine("; This is a comment") == "");
		EXPECT(parser.StripLine("path<staging> = /srv/uploads/; This is another comment") == "path<staging>=/srv/uploads/");
		EXPECT(parser.StripLine("[http]; This is a comment") == "[http]");

		// Allow the comment delimiter ';' inside a quoted value, but strip outer comments
		EXPECT(parser.StripLine("foobar=\"forever;alone\"; This is a comment") == "foobar=\"forever;alone\"");
	},

	CASE("IsValidSection correctly validates sections") {
		config::Parser parser;

		// All kinds of accepted section header strings
		EXPECT(parser.IsValidSection("[common]") == true);
		EXPECT(parser.IsValidSection("[http-common_123]") == true);
		EXPECT(parser.IsValidSection("[a]") == true);
		EXPECT(parser.IsValidSection("[1]") == true);
		EXPECT(parser.IsValidSection("[-1]") == true);
		EXPECT(parser.IsValidSection("[-]") == true);
		EXPECT(parser.IsValidSection("[_]") == true);

		// No spaces allowed; this function expects stripped input
		EXPECT(parser.IsValidSection(" [common]") == false);
		EXPECT(parser.IsValidSection("[common] ") == false);
		EXPECT(parser.IsValidSection("[foo bar]") == false);

		// No comments; this function expects stripped input
		EXPECT(parser.IsValidSection("[common];foobar") == false);
		EXPECT(parser.IsValidSection("[common] ;foobar") == false);

		// Whole string is comment
		EXPECT(parser.IsValidSection(";foobar [common]") == false);

		// Must have something in between square brackets
		EXPECT(parser.IsValidSection("[]") == false);
	},

	CASE("ParseSection correctly extracts section name") {
		config::Parser parser;

		EXPECT(parser.ParseSection("[common]") == "common");
		EXPECT(parser.ParseSection("[http]") == "http");
		EXPECT(parser.ParseSection("[http-common_123]") == "http-common_123");
		EXPECT(parser.ParseSection("[-]") == "-");
		EXPECT(parser.ParseSection("[_]") == "_");
		EXPECT(parser.ParseSection("[]") == "");
	},

	CASE("ParseSetting correctly parses valid and invalid settings") {
		config::Parser parser;
		config::SingleSetting setting;
		const char* section = "foobar";

		// Valid settings
		setting = parser.ParseSetting("basic_size_limit=26214400", section);
		EXPECT(setting.section == section);
		EXPECT(setting.key == "basic_size_limit");
		EXPECT(setting.override == "");
		EXPECT(setting.value == "26214400");

		setting = parser.ParseSetting("path=/tmp/", section);
		EXPECT(setting.section == section);
		EXPECT(setting.key == "path");
		EXPECT(setting.override == "");
		EXPECT(setting.value == "/tmp/");

		setting = parser.ParseSetting("path<production>=/srv/var/tmp/", section);
		EXPECT(setting.section == section);
		EXPECT(setting.key == "path");
		EXPECT(setting.override == "production");
		EXPECT(setting.value == "/srv/var/tmp/");

		setting = parser.ParseSetting("path<>=/srv/var/tmp/", section);
		EXPECT(setting.section == section);
		EXPECT(setting.key == "path");
		EXPECT(setting.override == "");
		EXPECT(setting.value == "/srv/var/tmp/");

		setting = parser.ParseSetting("name=\"http uploading\"", section);
		EXPECT(setting.section == section);
		EXPECT(setting.key == "name");
		EXPECT(setting.override == "");
		EXPECT(setting.value == "\"http uploading\"");

		setting = parser.ParseSetting("params=array,of,values", section);
		EXPECT(setting.section == section);
		EXPECT(setting.key == "params");
		EXPECT(setting.override == "");
		EXPECT(setting.value == "array,of,values");

		// Invalid settings (return object has empty strings)
		setting = parser.ParseSetting("=26214400", section);
		EXPECT(setting.section == "");
		EXPECT(setting.key == "");
		EXPECT(setting.override == "");
		EXPECT(setting.value == "");

		setting = parser.ParseSetting("path<production=/srv/var/tmp/", section);
		EXPECT(setting.section == "");
		EXPECT(setting.key == "");
		EXPECT(setting.override == "");
		EXPECT(setting.value == "");

		setting = parser.ParseSetting("pathproduction>=/srv/var/tmp/", section);
		EXPECT(setting.section == "");
		EXPECT(setting.key == "");
		EXPECT(setting.override == "");
		EXPECT(setting.value == "");

		setting = parser.ParseSetting("<path>production=/srv/var/tmp/", section);
		EXPECT(setting.section == "");
		EXPECT(setting.key == "");
		EXPECT(setting.override == "");
		EXPECT(setting.value == "");

		setting = parser.ParseSetting("<path>=/srv/var/tmp/", section);
		EXPECT(setting.section == "");
		EXPECT(setting.key == "");
		EXPECT(setting.override == "");
		EXPECT(setting.value == "");

		setting = parser.ParseSetting("<>=26214400", section);
		EXPECT(setting.section == "");
		EXPECT(setting.key == "");
		EXPECT(setting.override == "");
		EXPECT(setting.value == "");

		setting = parser.ParseSetting("=", section);
		EXPECT(setting.section == "");
		EXPECT(setting.key == "");
		EXPECT(setting.override == "");
		EXPECT(setting.value == "");

		setting = parser.ParseSetting("", section);
		EXPECT(setting.section == "");
		EXPECT(setting.key == "");
		EXPECT(setting.override == "");
		EXPECT(setting.value == "");
	},

	CASE("ConstructValueObject correctly sets the right type") {
		config::Parser parser;
		config::Item item;
		std::string stringValue;
		int64_t integerValue;
		double doubleValue;
		bool boolValue;
		std::vector<std::string> listValue;

		// Test integers
		stringValue = "26214400";
		integerValue = 26214400;
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::INTEGER);
		EXPECT(item.GetInteger() == integerValue);

		stringValue = "52428800";
		integerValue = 52428800;
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::INTEGER);
		EXPECT(item.GetInteger() == integerValue);

		stringValue = "2147483648";
		integerValue = 2147483648;
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::INTEGER);
		EXPECT(item.GetInteger() == integerValue);

		stringValue = "-2147483648";
		integerValue = -2147483648;
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::INTEGER);
		EXPECT(item.GetInteger() == integerValue);

		stringValue = "0";
		integerValue = 0;
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::INTEGER);
		EXPECT(item.GetInteger() == integerValue);

		stringValue = "1";
		integerValue = 1;
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::INTEGER);
		EXPECT(item.GetInteger() == integerValue);

		// Test 64-bit int overflow throws runtime exception
		stringValue = "99999999999999999999999999999999999999999999999999";
		integerValue = 0;
		EXPECT_THROWS_AS(parser.ConstructValueObject(stringValue), std::runtime_error);

		// Test doubles
		stringValue = "3.14";
		doubleValue = 3.14;
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::DOUBLE);
		EXPECT(item.GetDouble() == doubleValue);

		stringValue = "-3.14";
		doubleValue = -3.14;
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::DOUBLE);
		EXPECT(item.GetDouble() == doubleValue);

		stringValue = "0.0";
		doubleValue = 0.0;
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::DOUBLE);
		EXPECT(item.GetDouble() == doubleValue);

		// Test booleans
		stringValue = "true";
		boolValue = true;
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::BOOLEAN);
		EXPECT(item.GetBoolean() == boolValue);

		stringValue = "false";
		boolValue = false;
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::BOOLEAN);
		EXPECT(item.GetBoolean() == boolValue);

		stringValue = "yes";
		boolValue = true;
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::BOOLEAN);
		EXPECT(item.GetBoolean() == boolValue);

		stringValue = "no";
		boolValue = false;
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::BOOLEAN);
		EXPECT(item.GetBoolean() == boolValue);

		// Test lists
		stringValue = "array,of,values";
		listValue = {"array", "of", "values"};
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::LIST);
		EXPECT(item.GetList() == listValue);

		// Test strings
		stringValue = "/srv/var/tmp/";
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::STRING);
		EXPECT(item.GetString() == stringValue);

		// A quoted string should strip quotes, and not convert to list
		// even if there is a comma within.
		stringValue = "\"hello there, ftp uploading\"";
		std::string strippedStringValue = "hello there, ftp uploading";
		item = parser.ConstructValueObject(stringValue);
		EXPECT(item.GetValueType() == config::ValueType::STRING);
		EXPECT(item.GetString() == strippedStringValue);
	},
};

int main(int argc, char* argv[]) {
	return lest::run(specification, argc, argv);
}
