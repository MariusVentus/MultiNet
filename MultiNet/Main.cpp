#include <iostream>
#include "Topology.h"
#include "NeuralNet.h"

int main() {


	float T1[4] = { 0.0f,1.0f,0.0f,1.0f };
	float T2[4] = { 0.0f,0.0f,1.0f,1.0f };
	float outVal[4] = { 0.0f,1.0f,1.0f,0.0f };

	Topology Top(3);
	std::cout << "Hello! \n";

	Top.CreateMonoLayer(0, 2, 4);
	Top.CreateMonoLayer(1, 2, 4);
	Top.CreateMonoLayer(2, 1, 4);

	NeuralNet NN(Top);

	std::cin.get();
	unsigned zip = 0;

	//Crappy Quick XOR Test. 
	for (unsigned bleh = 0; bleh < 20000; bleh++) {
		std::vector<float> TmpIn;
		std::vector<float> TmpOut;
		std::vector<float> resultVals;

		TmpIn.push_back(T1[zip]);
		TmpIn.push_back(T2[zip]);
		TmpOut.push_back(outVal[zip]);

		//Feed
		NN.FeedForward(TmpIn);
		//Results
		NN.GetResults(resultVals);

		std::cout << "In1: " << T1[zip] << " In2: " << T2[zip] << " Real Out: " << resultVals[0] << " ";

		if (resultVals[0] >= .5) {
			std::cout << " Effective Out: " << 1 << " ";
		}
		else if (resultVals[0] < .5) {
			std::cout << " Effective Out: " << 0 << " ";
		}


		//Correct
		NN.BackProp(TmpOut);
		std::cout << "Target: " << TmpOut[0] << " \n";
		std::cout << "Error: " << NN.GetError() << " \n";


		zip++;
		if (zip >= 4) {
			zip = 0;
		}
	}



	std::cin.ignore();
	std::cin.get();
	return 0;
}