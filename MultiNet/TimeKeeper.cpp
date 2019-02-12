#include "TimeKeeper.h"

using namespace std::chrono;

TimeKeeper::TimeKeeper()
{
	last = steady_clock::now();
}

float TimeKeeper::Mark()
{
	const auto old = last;
	last = steady_clock::now();
	const duration<float> epochTime = last - old;
	return epochTime.count();
}


