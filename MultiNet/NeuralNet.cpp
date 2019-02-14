#include <assert.h>
#include <iostream>
#include "NeuralNet.h"

NeuralNet::NeuralNet(const Topology & netTop)
{
	unsigned numLayers = netTop.GetLayerCount();
	for (unsigned layerNum = 0; layerNum < numLayers; layerNum++) {
		m_layers.push_back(Layer());
		std::cout << "Layer" << layerNum << ": ";
		//Always add +1 to the Num Inputs, because the Bias is added outside the Topology.
		unsigned numInputs = layerNum == 0 ? 0 : netTop.GetNCount(layerNum - 1) + 1;

		for (unsigned neuronNum = 0; neuronNum < netTop.GetNCount(layerNum); neuronNum++) {
			m_layers.back().push_back(Neuron(numInputs, neuronNum, netTop.GetNType(layerNum, neuronNum)));
		}
		//Add Bias - Fun fact, the NCount of your own layer does not include the bias, as it is added last!
		m_layers.back().push_back(Neuron(0, netTop.GetNCount(layerNum), 0));
		//Set Layer Bias to one. 
		m_layers.back().back().SetOutputVal(1.0);
		std::cout << "\n";

	}
}

void NeuralNet::FeedForward(const std::vector<float>& inputVals)
{

}

void NeuralNet::BackProp(const std::vector<float>& targetVals)
{

}

void NeuralNet::GetResults(std::vector<float>& resultVals) const
{

}
