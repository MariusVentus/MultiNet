#include "Neuron.h"
#include <cmath>
#include <iostream>
#include <random>

Neuron::Neuron(unsigned numInputs, unsigned myIndex, unsigned myType)
{
	std::random_device rd;
	std::mt19937 rng(rd());
	std::normal_distribution<float> nDist(0, 1);

	for (unsigned cnxn = 0; cnxn < numInputs; ++cnxn) {
		m_inputWeights.push_back(Synapse());
		m_inputWeights.back().weight = nDist(rng);
	}
	m_myIndex = myIndex;
	m_myType = myType;
	std::cout << "N" << m_myType << " ";
}

void Neuron::CalcHiddenGradients(const Layer& nextLayer)
{
	float dow = SumDOW(nextLayer);
	m_gradient = dow*Neuron::TransferFunctionDerivative(m_myType, m_inputVals);
}

void Neuron::CalcOutputGradients(float targetVal)
{
	float delta = targetVal - m_outputVal;
	m_gradient = delta * Neuron::TransferFunctionDerivative(m_myType, m_inputVals);
}

void Neuron::FeedForward(const Layer& prevLayer)
{
	float sum = 0.0f;

	for (unsigned n = 0; n < prevLayer.size(); ++n) {
		sum += prevLayer[n].GetOutputVal() * m_inputWeights[n].weight;
	}
	m_inputVals = sum;
	m_outputVal = Neuron::TransferFunction(m_myType, sum);
}

void Neuron::UpdateInputWeights(Layer& prevLayer)
{
	for (unsigned n = 0; n < prevLayer.size(); n++) {
		float oldDeltaWeight = m_inputWeights[n].deltaWeight;

		float newDeltaWeight =
			SettingManager::eta*prevLayer[n].GetOutputVal()*m_gradient + SettingManager::alpha*oldDeltaWeight;

		m_inputWeights[n].deltaWeight = newDeltaWeight;
		m_inputWeights[n].weight += newDeltaWeight;
	}
}

float Neuron::SumDOW(const Layer& nextLayer)
{
	float sum = 0.0;
	for (unsigned n = 0; n < nextLayer.size() - 1; ++n) {
		sum += nextLayer[n].m_inputWeights[m_myIndex].weight*nextLayer[n].m_gradient;
	}
	return sum;
}

float Neuron::TransferFunction(unsigned inType, float x)
{
	switch (inType) {
		//Bias - If something goes wrong, still returns 1. 
	case 0: return 1.0f;
		//Linear
	case 1:	return x;
		//Sigmoidal
	case 2:	return 1.0f / (1.0f + exp(-x));
		//Tanh
	case 3:	return tanh(x);
		//Leaky Relu
	case 4:
		if (x >= 0.0f) {
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

float Neuron::TransferFunctionDerivative(unsigned inType, float x)
{
	switch (inType) {
		//Bias
	case 0: return 0.0f;
		//Linear
	case 1:	return 1.0f;
		//Sigmoidal
	case 2:	return (1.0f / (1.0f + exp(-x)))*(1.0f - (1.0f / (1.0f + exp(-x))));
		//Tanh
	case 3:	return (1.0f - tanh(x) * tanh(x));
		//Leaky Relu
	case 4:
		if (x >= 0.0f) {
			return 1.0f;
		}
		else if (x < 0.0f) {
			return SettingManager::leak;
		}
		else {
			return 1.0f;
		}
		//Gaussian
	case 5:	return -2.0f * x*exp(-(x*x));


		//If nothing else, run the neurons as they were originally written. 
	default: return (1.0f - tanh(x) * tanh(x));

	}
}
