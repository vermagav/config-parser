/*
 * A config Item object represents a single configuration value.
 */

#ifndef CONFIG_ITEM_H_
#define CONFIG_ITEM_H_

#include <iostream>
#include <vector>

#include "errors.h"

namespace config {

// The different types supported by a config item.
enum ValueType {
	STRING,
	BOOLEAN,
	INTEGER,
	DOUBLE,
	LIST
};

// A single configuration item with its value type.
class Item {
	public:
		ValueType GetValueType();
		std::string GetString();
		bool GetBoolean();
		int64_t GetInteger();
		double GetDouble();
		std::vector<std::string> GetList();

		void SetString(std::string);
		void SetBoolean(bool);
		void SetInteger(int64_t);
		void SetDouble(double);
		void SetList(std::vector<std::string>);

	private:
		ValueType valueType;
		std::string stringValue;
		bool booleanValue;
		int64_t	integerValue;
		double doubleValue;
		std::vector<std::string> listValue;
};

} // namespace Config

#endif // CONFIG_ITEM_H_
