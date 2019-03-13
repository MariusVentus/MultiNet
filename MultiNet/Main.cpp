#include <iostream>
#include "Topology.h"
#include "NeuralNet.h"
#include "DataHandler.h"
#include "RandHandler.h"
#include <assert.h>

int main() {
	SettingManager Settings("Settings\\Settings.txt");
	DataHandler input(Settings, "Data\\input.txt");
	DataHandler output(Settings, "Data\\output.txt");
	assert(input.GetBuffSize() == output.GetBuffSize());

	Topology Top("Topologies\\Topology.txt", input.GetRowSize(), output.GetRowSize());
	NeuralNet NN(Top, Settings);
	std::cin.get();

	RandHandler rng;
	bool keepTraining = true;
	while (keepTraining) {
		for (unsigned epochs = 0; epochs < 1; ++epochs) {
			while (!input.GetEoF()) {
				if (Settings.GetRandTrainData() == true) {
					rng.GenNShuffle(input.GetBuffSize());
				}
				else {
					rng.GenSelectArray(input.GetBuffSize());
				}
				for (unsigned i = 0; i < input.GetBuffSize(); i++) {
					assert(input.GetBuffSize() == output.GetBuffSize());
					std::vector<float> resultVals;
					//Feed
					NN.FeedForward(input.GetRowX(rng.GetSelect(i)));
					//Results
					NN.GetResults(resultVals);
					//Correct
					NN.BackProp(output.GetRowX(rng.GetSelect(i)));

					//Display
					std::cout << "\n";
					std::cout << "Input: ";
					for (unsigned j = 0; j < input.GetRowSize(); j++) {
						std::cout << input.GetRowX(rng.GetSelect(i))[j] << " ";
					}
					std::cout << "\n";

					std::cout << "Output: ";
					for (unsigned j = 0; j < output.GetRowSize(); j++) {
						std::cout << resultVals[j] << " ";
					}
					std::cout << "\n";

					std::cout << "Target: ";
					for (unsigned j = 0; j < output.GetRowSize(); j++) {
						std::cout << output.GetRowX(rng.GetSelect(i))[j] << " ";
					}
					std::cout << "\n";
				}
				input.ReloadBuffer();
				output.ReloadBuffer();
			}
			input.ResetEoF();
		}
		std::cin.get();
	}








	std::cin.ignore();
	std::cin.get();
	return 0;
}