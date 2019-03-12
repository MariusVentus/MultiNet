#include "RandHandler.h"
#include <algorithm>

RandHandler::RandHandler()
	:
	rng(rd())
{
}

void RandHandler::GenNShuffle(const unsigned& buffSize)
{
	std::uniform_int_distribution<unsigned> rDist(0, buffSize - 1);
	unsigned lhs = rDist(rng);
	unsigned rhs = rDist(rng);
	while (lhs == rhs) {
		rhs = rDist(rng);
	}

	GenSelectArray(buffSize);

	for (unsigned nSwap = 0; nSwap < buffSize; nSwap++) {
		std::swap(selectArray[lhs], selectArray[rhs]);
		lhs = rDist(rng);
		rhs = rDist(rng);
		while (lhs == rhs) {
			rhs = rDist(rng);
		}
	}
}

void RandHandler::GenSelectArray(const unsigned& arrSize)
{
	selectArray.clear();
	for (unsigned i = 0; i < arrSize; i++) {
		selectArray.emplace_back(i);
	}
}
