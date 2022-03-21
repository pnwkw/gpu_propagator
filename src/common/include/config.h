#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#include <cstddef>
#include <string>

namespace common {
	constexpr std::size_t windowWidth = 1920;
	constexpr std::size_t windowHeight = 1080;
    constexpr bool isBenchmark = false;
	constexpr bool headless = isBenchmark;

    constexpr std::size_t scale_s = 4;
    constexpr std::size_t scale_tp = 4;

	constexpr std::size_t pointsPerTrack = 256;
    constexpr std::size_t runs = 10;

	extern std::size_t samples;
	extern std::string method;
	extern std::size_t deviceID;
	extern std::string resultsDir;

	void configRead(const std::string &name);

#ifdef NDEBUG
	constexpr bool isDebug = false;
#else
	constexpr bool isDebug = true;
#endif
}// namespace common

#endif//COMMON_CONFIG_H
