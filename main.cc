#include <iostream>
#include <iterator>
#include <sstream>

#include "config/handler.h"

// Helper function to print settings of heterogeneous types.
void printSetting(config::Item* setting) {
	if (setting == NULL) {
		std::cout << "[NONE]:\tSetting not found.\n";
	} else {
		switch (setting->GetValueType()) {
			case config::ValueType::STRING:
			std::cout << "[STR]:\t" << setting->GetString() << "\n";
			break;

			case config::ValueType::BOOLEAN:
			std::cout << "[BOOL]:\t" << setting->GetBoolean() << "\n";
			break;

			case config::ValueType::INTEGER:
			std::cout << "[INT]:\t" << setting->GetInteger() << "\n";
			break;

			case config::ValueType::DOUBLE:
			std::cout << "[DBL]:\t" << setting->GetDouble() << "\n";
			break;

			case config::ValueType::LIST:
			std::cout << "[LIST]:\t{ ";
			std::vector<std::string> list = setting->GetList();
			unsigned int i = 0;
			for (; i < list.size()-1; i++) {
				std::cout << list[i] << ", ";
			}
			std::cout << list[i] << " }\n";
			break;
		}
	}
}

int main() {
	try {
		// Get a config handler
		config::Handler handler;

		// Load a config file
		handler.Load("sample.ini", {"production", "ubuntu"});

		// Output some settings to standard output
		config::Item* setting;

		setting = handler.Get("common.paid_users_size_limit");
		std::cout << "\n\nTest print directly: " << setting->GetInteger() << "\n";

		setting = handler.Get("common.paid_users_size_limit");
		printSetting(setting);

		setting = handler.Get("ftp.name");
		printSetting(setting);

		setting = handler.Get("http.params");
		printSetting(setting);

		setting = handler.Get("ftp.foobar123");
		printSetting(setting);

		setting = handler.Get("ftp.enabled");
		printSetting(setting);

		setting = handler.Get("ftp.path");
		printSetting(setting);

		// Add more here...



		std::unordered_map<std::string, config::Item>* sectionMap = handler.GetSection("common");
		if (sectionMap == NULL) {
			std::cout << "Section not found.\n";
		} else {
			std::cout << "\n\nPrinting ALL keys and values via GetSection():\n";
			for (auto kv : *sectionMap) {
				std::cout << "[KEY]:\t" << kv.first << ":\n";
				printSetting(&kv.second);
			}
		}

		// ...or here



	} catch (std::exception e) {
		std::cout << "Encountered error: " << e.what() << "\n";
	}

	return 0;
}
