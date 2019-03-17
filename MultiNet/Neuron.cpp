#include "Neuron.h"
#include <cmath>
#include <iostream>

Neuron::Neuron(const SettingManager& nSet, unsigned numInputs, unsigned numOutputs, unsigned myIndex, unsigned myType)
	:
	m_neuronSet(nSet),
	m_myType(myType),
	m_myIndex(myIndex)
{
	std::random_device rd;
	std::mt19937 rng(rd());

	for (unsigned cnxn = 0; cnxn < numInputs; ++cnxn) {
		m_inputWeights.push_back(Synapse());
		m_inputWeights.back().weight = RandomWeight(rng, numInputs, numOutputs);
	}
	std::cout << "N" << m_myType << " ";
}

void Neuron::CalcHiddenGradients(const Layer& nextLayer)
{
	float dow = SumDOW(nextLayer);
	m_gradient = dow*Neuron::TransferFunctionDerivative(m_myType, m_inputVals);
	HardClipping();
}

void Neuron::CalcOutputGradients(float targetVal)
{
	float delta = targetVal - m_outputVal;
	m_gradient = delta * Neuron::TransferFunctionDerivative(m_myType, m_inputVals);
	HardClipping();
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

void Neuron::NormClipping(const float& inNorm)
{
	if (inNorm != 0.0f && inNorm > m_neuronSet.GetClipThreshold()) {
		m_gradient = m_gradient*(m_neuronSet.GetClipThreshold() / inNorm);
	}
}

void Neuron::UpdateInputWeights(Layer& prevLayer)
{
	for (unsigned n = 0; n < prevLayer.size(); n++) {
		float oldDeltaWeight = m_inputWeights[n].deltaWeight;

		float newDeltaWeight =
			m_neuronSet.GetEta()*prevLayer[n].GetOutputVal()*m_gradient + m_neuronSet.GetAlpha()*oldDeltaWeight;

		m_inputWeights[n].deltaWeight = newDeltaWeight;
		m_inputWeights[n].weight += newDeltaWeight;
	}
}

void Neuron::HardClipping(void)
{
	if (m_neuronSet.GetClipping() == SettingManager::Clipping::HardClip) {
		if (m_gradient > m_neuronSet.GetClipThreshold()) {
			m_gradient = m_neuronSet.GetClipThreshold();
		}
		else if (m_gradient < -m_neuronSet.GetClipThreshold()) {
			m_gradient = -m_neuronSet.GetClipThreshold();
		}
	}
}

float Neuron::SumDOW(const Layer& nextLayer) const
{
	float sum = 0.0;
	for (unsigned n = 0; n < nextLayer.size() - 1; ++n) {
		sum += nextLayer[n].m_inputWeights[m_myIndex].weight*nextLayer[n].m_gradient;
	}
	return sum;
}

float Neuron::RandomWeight(std::mt19937& rng, const unsigned& numInputs, const unsigned& numOutputs)
{
	std::normal_distribution<float> nDist(0, 1);

	switch (m_neuronSet.GetInit()) {
	case SettingManager::Initialization::Normal:
		return nDist(rng);
	case SettingManager::Initialization::XavierSimple:
		if (m_myType == 4) {
			return nDist(rng)*(sqrt(2.0f / float(numInputs)));
		}
		else {
			return nDist(rng)*(sqrt(1.0f / float(numInputs)));
		}
	case SettingManager::Initialization::XavierFull:
		if (m_myType == 4) {
			return nDist(rng)*(sqrt(4.0f / float(numInputs + numOutputs)));
		}
		else {
			return nDist(rng)*(sqrt(2.0f / float(numInputs + numOutputs)));
		}
	default:
		return nDist(rng);
	}
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
			return m_neuronSet.GetLeak()*x;
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
			return m_neuronSet.GetLeak();
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
