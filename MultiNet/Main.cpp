#include "Cortex.h"
#include "IOCustom.h"
#include <iostream>
#include <string>

int main() {
	std::cout << "Multinet V1.0-alpha \n";
	std::cout << "Author: MariusVentus \n\n";

	Cortex Brain("Settings\\Settings.txt", "Topologies\\Topology.txt", "Data\\input.txt", "Data\\output.txt");
	IOCustom IO;

	do {
		std::cout << "Training.\n";
		Brain.Train(IO.HowMany("Epochs"));

		if (IO.YnQ("Would you like to test against reserved data?")) {
			std::cout << "\nTesting.\n";
			Brain.Test();
		}

		while (IO.YnQ("Would you like to enter a manual input?")) {
			std::string manInLine;
			std::cout << "Please enter the input, separating values with Commas\n";
			std::getline(std::cin, manInLine);
			IO.CleanLine(manInLine);
			Brain.ManualRun(manInLine);

		}

	} while (IO.YnQ("Continue Training?"));

	if (IO.YnQ("Do you want to save the Network?")) {
		Brain.Save();
	}

	std::cout << "Complete. Press Any Key to end." << std::endl;

	std::cin.get();
	return 0;
}