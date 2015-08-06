/*
 * A Handler object is the entry point that is used to access config data.
 */

#ifndef CONFIG_HANDLER_H_
#define CONFIG_HANDLER_H_

#include <unordered_map>
#include <vector>

#include "parser.h"

namespace config {

// For readability
using std::string;
using std::unordered_map;
using std::vector;

extern const char SECTION_DELIM;

class Handler {
  private:
	// A map to hold each individual setting in memory for O(1) access.
	unordered_map<string, config::Item> settingsSingle;

	// A map to hold a symbolized hashmap of every section in memory for O(1) access.
	unordered_map<string, unordered_map<string, config::Item>> settingsSection;

  public:
	// The main loader function that takes a filename and a list of overrides.
	// This function will return true if the load succeeded, throw an exception
	// if it encounters a critical error (the caller should try-catch), and
	// return false if the load did not succeed due to a recoverable error.
	// The current policy of this function is to return false upon encountering
	// files with invalid settings. This is a design decision, and could be changed
	// to be more tolerant and skip those settings instead; but doing so would
	// expose the caller to risks of expecting settings that may not exist.
	bool Load(string, vector<string>);

	// Get an individual setting. Returns NULL if not found.
	config::Item* Get(string);

	// Get a map of all settings in a section. Returns NULL if not found.
	unordered_map<string, config::Item>* GetSection(string);
};

} // namespace Config

#endif // CONFIG_HANDLER_H_
