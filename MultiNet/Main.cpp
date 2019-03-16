#include "Cortex.h"


#include <iostream>

int main() {
	Cortex Brain("Settings\\Settings.txt", "Topologies\\Topology.txt", "Data\\input.txt", "Data\\output.txt");



	bool keepTraining = true;
	while (keepTraining) {
		std::cout << "How many Epochs?\n";
		unsigned bleh = 0;
		std::cin >> bleh;
		Brain.Train(bleh);
		std::cout << "Testing.\n";
		std::cin.ignore();
		std::cin.get();
		Brain.Test();
	}

	std::cin.ignore();
	std::cin.get();
	return 0;
}