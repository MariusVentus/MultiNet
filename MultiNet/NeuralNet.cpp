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
	assert(inputVals.size() == m_layers[0].size() - 1);
	// Add -1 since t does not need to check Bias Neuron
	for (unsigned i = 0; i < inputVals.size(); i++) {
		m_layers[0][i].SetOutputVal(inputVals[i]);
	}
	//Start from one, as the Input Layer does not need to be Fed.
	for (unsigned layerNum = 1; layerNum < m_layers.size(); ++layerNum) {
		Layer& prevLayer = m_layers[layerNum - 1];
		for (unsigned n = 0; n < m_layers[layerNum].size() - 1; ++n) {
			m_layers[layerNum][n].FeedForward(prevLayer);
		}
	}
}

void NeuralNet::BackProp(const std::vector<float>& targetVals)
{

}

void NeuralNet::GetResults(std::vector<float>& resultVals) const
{
	resultVals.clear();
	// Don't forget to -1, because no Bias on the output. 
	for (unsigned n = 0; n < m_layers.back().size() - 1; n++) {
		resultVals.push_back(m_layers.back()[n].GetOutputVal());
	}
}
