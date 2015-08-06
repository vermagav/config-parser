#include <iostream>
#include <stdexcept>

#include "../common/lest.hpp"
#include "item.h"

const lest::test specification[] = {
	CASE("A string item is retricted to its type") {
		std::string i = "foobar";
		config::Item item;
		item.SetString(i);

		// Able to retrieve correct value of expected type
		EXPECT(config::ValueType::STRING == item.GetValueType());
		EXPECT(i == item.GetString());

		// Throw runtime error when retrieving incompatible types
		EXPECT_THROWS_AS(item.GetBoolean(), std::runtime_error);
		EXPECT_THROWS_AS(item.GetInteger(), std::runtime_error);
		EXPECT_THROWS_AS(item.GetDouble(), std::runtime_error);
		EXPECT_THROWS_AS(item.GetList(), std::runtime_error);
	},

	CASE("A boolean item is retricted to its type") {
		bool i = true;
		config::Item item;
		item.SetBoolean(i);

		// Able to retrieve correct value of expected type
		EXPECT(config::ValueType::BOOLEAN == item.GetValueType());
		EXPECT(i == item.GetBoolean());

		// Throw runtime error when retrieving incompatible types
		EXPECT_THROWS_AS(item.GetString(), std::runtime_error);
		EXPECT_THROWS_AS(item.GetInteger(), std::runtime_error);
		EXPECT_THROWS_AS(item.GetDouble(), std::runtime_error);
		EXPECT_THROWS_AS(item.GetList(), std::runtime_error);
	},

	CASE("An integer item is retricted to its type") {
		int64_t	i = 12345;
		config::Item item;
		item.SetInteger(i);

		// Able to retrieve correct value of expected type
		EXPECT(config::ValueType::INTEGER == item.GetValueType());
		EXPECT(i == item.GetInteger());

		// Throw runtime error when retrieving incompatible types
		EXPECT_THROWS_AS(item.GetString(), std::runtime_error);
		EXPECT_THROWS_AS(item.GetBoolean(), std::runtime_error);
		EXPECT_THROWS_AS(item.GetDouble(), std::runtime_error);
		EXPECT_THROWS_AS(item.GetList(), std::runtime_error);
	},

	CASE("A double item is retricted to its type") {
		double i = 3.14;
		config::Item item;
		item.SetDouble(i);

		// Able to retrieve correct value of expected type
		EXPECT(config::ValueType::DOUBLE == item.GetValueType());
		EXPECT(i == item.GetDouble());

		// Throw runtime error when retrieving incompatible types
		EXPECT_THROWS_AS(item.GetString(), std::runtime_error);
		EXPECT_THROWS_AS(item.GetBoolean(), std::runtime_error);
		EXPECT_THROWS_AS(item.GetInteger(), std::runtime_error);
		EXPECT_THROWS_AS(item.GetList(), std::runtime_error);
	},

	CASE("A list item is retricted to its type") {
		std::vector<std::string> i = {
			std::string("foo"),
			std::string("bar"),
			std::string("baz")
		};
		config::Item item;
		item.SetList(i);

		// Able to retrieve correct value of expected type
		EXPECT(config::ValueType::LIST == item.GetValueType());
		EXPECT(i == item.GetList());

		// Throw runtime error when retrieving incompatible types
		EXPECT_THROWS_AS(item.GetString(), std::runtime_error);
		EXPECT_THROWS_AS(item.GetBoolean(), std::runtime_error);
		EXPECT_THROWS_AS(item.GetInteger(), std::runtime_error);
		EXPECT_THROWS_AS(item.GetDouble(), std::runtime_error);
	}
};

int main(int argc, char* argv[]) {
	return lest::run(specification, argc, argv);
}
