#include "config.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace common {
	std::size_t samples;
	std::string method;
	std::size_t deviceID;
	std::string resultsDir;

	void configRead(const std::string &name) {
		std::ifstream i(name);
		nlohmann::json j;
		i >> j;

		samples = j["samples"];

		method = j["method"];

		if (j.find("deviceID") != j.end()) {
			deviceID = j["deviceID"];
		} else {
			deviceID = 0;
		}

		if (j.find("resultsDir") != j.end()) {
			resultsDir = j["resultsDir"];
		} else {
			resultsDir = ".";
		}
	}
}// namespace common
