#include <stdexcept>

#include "item.h"

namespace config {

ValueType Item::GetValueType() {
	return valueType;
}

std::string Item::GetString() {
	if (valueType != ValueType::STRING) {
		// Note: to be tolerant, we could permit returning string for
		// all types as string is the base type. But this invalidates
		// the type contract and exposes the caller to usage risk.
		throw std::runtime_error(errors::TYPE_MISMATCH);
	}
	return stringValue;
}

bool Item::GetBoolean() {
	if (valueType != ValueType::BOOLEAN) {
		throw std::runtime_error(errors::TYPE_MISMATCH);
	}
	return booleanValue;
}

int64_t Item::GetInteger() {
	if (valueType != ValueType::INTEGER) {
		throw std::runtime_error(errors::TYPE_MISMATCH);
	}
	return integerValue;
}

double Item::GetDouble() {
	if (valueType != ValueType::DOUBLE) {
		throw std::runtime_error(errors::TYPE_MISMATCH);
	}
	return doubleValue;
}

std::vector<std::string> Item::GetList() {
	if (valueType != ValueType::LIST) {
		throw std::runtime_error(errors::TYPE_MISMATCH);
	}
	return listValue;
}

void Item::SetString(std::string in) {
	valueType = ValueType::STRING;
	stringValue = in;
}

void Item::SetBoolean(bool in) {
	valueType = ValueType::BOOLEAN;
	booleanValue = in;
}

void Item::SetInteger(int64_t in) {
	valueType = ValueType::INTEGER;
	integerValue = in;
}

void Item::SetDouble(double in) {
	valueType = ValueType::DOUBLE;
	doubleValue = in;
}

void Item::SetList(std::vector<std::string> in) {
	valueType = ValueType::LIST;
	listValue = in;
}

} // namespace config
