#include <iostream>
#include "Topology.h"
#include "NeuralNet.h"

int main() {
	Topology Top(4);
	std::cout << "Hello! \n";

	Top.CreateMonoLayer(0, 3, 4);
	Top.CreateMixedLayer(1, 5);
	Top.CreateMonoLayer(2, 5, 5);

	NeuralNet NN(Top);




	std::cin.get();
	return 0;
}