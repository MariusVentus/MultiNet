#pragma once
#include <chrono>

class TimeKeeper {
public:
	TimeKeeper();
	float Mark();
private:
	std::chrono::steady_clock::time_point last;
};