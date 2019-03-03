#include <iostream>
#include "Topology.h"
#include "NeuralNet.h"
#include "DataHandler.h"
#include <assert.h>

int main() {
	SettingManager Settings("Settings\\Settings.txt");
	DataHandler input(Settings, "Data\\inputDefault.txt");
	DataHandler output(Settings, "Data\\outputDefault.txt");
	assert(input.GetBuffSize() == output.GetBuffSize());

	Topology Top(3);
	Top.CreateMonoLayer(0, input.GetRowSize(), 1);
	Top.CreateMonoLayer(1, 2, 3);
	Top.CreateMonoLayer(2, output.GetRowSize(), 3);
	NeuralNet NN(Top, Settings);
	std::cin.get();

	bool keepTraining = true;
	while (keepTraining) {
		for (unsigned epochs = 0; epochs < 10; ++epochs) {
			while (!input.GetEoF()) {
				for (unsigned i = 0; i < input.GetBuffSize(); i++) {
					assert(input.GetBuffSize() == output.GetBuffSize());
					std::vector<float> resultVals;
					//Feed
					NN.FeedForward(input.GetRowX(i));
					//Results
					NN.GetResults(resultVals);
					//Correct
					NN.BackProp(output.GetRowX(i));

					//Display
					std::cout << "\n";
					std::cout << "Input: ";
					for (unsigned j = 0; j < input.GetRowSize(); j++) {
						std::cout << input.GetRowX(i)[j] << " ";
					}
					std::cout << "\n";

					std::cout << "Output: ";
					for (unsigned j = 0; j < output.GetRowSize(); j++) {
						std::cout << resultVals[j] << " ";
					}
					std::cout << "\n";

					std::cout << "Target: ";
					for (unsigned j = 0; j < output.GetRowSize(); j++) {
						std::cout << output.GetRowX(i)[j] << " ";
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