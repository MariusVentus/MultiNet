#pragma once
#include <vector>
#include <random>

class RandHandler {
public:
	RandHandler();
	void GenNShuffle(const unsigned& buffSize);
	void GenSelectArray(const unsigned& arrSize);
	unsigned GetSelect(const unsigned& inX) const { return selectArray[inX]; }

private:
	//Vars
	std::random_device rd;
	std::mt19937 rng;
	std::vector<unsigned> selectArray;
};