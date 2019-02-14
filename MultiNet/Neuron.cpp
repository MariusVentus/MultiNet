#include "Neuron.h"
#include <iostream>

Neuron::Neuron(unsigned numInputs, unsigned myIndex, unsigned myType)
{
	for (unsigned cnxn = 0; cnxn < numInputs; ++cnxn) {
		m_inputWeights.push_back(Synapse());
		m_inputWeights.back().weight = RandomWeightStandard();
	}
	m_myIndex = myIndex;
	m_myType = myType;
	std::cout << "N" << m_myType << " ";
}

void Neuron::CalcHiddenGradients(const Layer & nextLayer)
{
}

void Neuron::CalcOutputGradients(float targetVal)
{
}

void Neuron::FeedForward(const Layer & prevLayer)
{
	float sum = 0.0f;

	for (unsigned n = 0; n < prevLayer.size(); ++n) {
		sum += prevLayer[n].GetOutputVal() * m_inputWeights[n].weight;
	}
	m_inputVals = sum;
	m_outputVal = Neuron::TransferFunction(m_myType, sum);
}

void Neuron::UpdateInputWeights(Layer & prevLayer)
{
}

float Neuron::TransferFunction(unsigned inType, float x)
{
	switch (inType) {
		//Bias - If something goes wrong, still returns 1. 
	case 0: return 1.0;
		//Linear
	case 1:	return x;
		//Sigmoidal
	case 2:	return 1 / (1 + exp(-x));
		//Tanh
	case 3:	return tanh(x);
		//Leaky Relu
	case 4:
		if (x >= 0.0) {
			return x;
		}
		else {
			return SettingManager::leak*x;
		}
		//Gaussian
	case 5:	return exp(-(x*x));

		//If nothing else, run the neurons as they were originally written. 
	default: return tanh(x);
	}
}
