/*
 * Error string definitions.
 */

#ifndef CONFIG_ERRORS_H_
#define CONFIG_ERRORS_H_

namespace config {
namespace errors {

// Error strings go here, placed alphabetically.
static const char* FILE_OPEN = "Unable to open config file";
static const char* SETTING_MAX_INTEGER = "The config file contained an integer larger than the supported max (64-bit signed)";
static const char* SETTING_MAX_DOUBLE = "The config file contained a floating point value larger than the supported max";
static const char* TYPE_MISMATCH = "This config item is not of this value type";

} // namespace errors
} // namespace Config

#endif // CONFIG_ERRORS_H_
