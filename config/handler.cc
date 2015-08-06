#include "handler.h"

namespace config {

// For readability
using std::string;
using std::unordered_map;
using std::vector;

const char SECTION_DELIM = '.';

bool Handler::Load(string filename, vector<string> overrides) {
	config::Parser parser;

	// Make a map of overrides for quick look-up
	unordered_map<string, bool> overridesMap;
	for (const auto& override : overrides) {
		overridesMap[override] = true;
	}

	// Make another temporary map to store only keys which
	// were overriden.
	unordered_map<string, bool> overridenKeys;

	// Step 1: Open file and convert it into a
	// vector of strings for each line.
	vector<string> lines = parser.ParseFile(filename);

	string section = "";
	// For each line in the config file...
	for (auto line : lines) {
		// Step 2: Strip the line of all whitespace and comments.
		// Note: this will not strip whitespaces inside quoted values.
		line = parser.StripLine(line);

		// Skip empty lines.
		if (line.length() == 0) {
			continue;
		}

		if (parser.IsValidSection(line)) {
			// Step 3: Is this line a valid section? If so, extract and set.
			section = parser.ParseSection(line);
		} else {
			// Step 4: Parse the setting into disparate strings.
			// SingleSetting contains: section, key, override, and value.
			config::SingleSetting setting = parser.ParseSetting(line, section);

			// Return false if this is a malformed setting.
			if (setting.key == "") {
				return false;
			}

			// Step 5: Construct a Item object from the value string.
			config::Item finalValue = parser.ConstructValueObject(setting.value);

			// Step 6: Compute concatenated section key
			string sectionKey = setting.section + SECTION_DELIM + setting.key;

			// Step 7: Process overrides
			bool isOverride = (overridesMap.count(setting.override) > 0);
			if (isOverride) {
				// Current is override, remember that we processed this
				overridenKeys[sectionKey] = true;
			} else {
				// Current isn't override, but is this a key that was
				// previously overriden? If so, don't do anything.
				if (overridenKeys.count(sectionKey) > 0) {
					continue;
				}
			}

			// Step 8: Now that we have a config item, add but only if
			// override matches or is none.
			if (isOverride || setting.override == "") {
				// -- A. Individual Map for fast access of setting
				settingsSingle[sectionKey] = finalValue;

				// -- B. Section Map for fast access of section
				if (settingsSection.count(section) == 0) {
					settingsSection[section] = unordered_map<string, config::Item>();
				}
				settingsSection[section][setting.key] = finalValue;
			}
		}
	}
	return true;
}

config::Item* Handler::Get(string key) {
	if (settingsSingle.count(key) > 0) {
		return &settingsSingle[key];
	}
	return NULL;
}

unordered_map<string, config::Item>* Handler::GetSection(string section) {
	if (settingsSection.count(section) > 0) {
		return &settingsSection[section];
	}
	return NULL;
}

} // namespace config
