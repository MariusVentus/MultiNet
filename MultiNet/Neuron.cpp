#include "Neuron.h"
#include <algorithm>
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

	unsigned effectiveInputs = numInputs;
	if (nSet.GetSimpleRecurrency() && myType != 99 && myType != 0) {
		if (!nSet.isLinearRestricted() || (myType != 1 && myType != 4)) {
			effectiveInputs++;
		}
	}

	for (unsigned cnxn = 0; cnxn < effectiveInputs; ++cnxn) {
		m_inputWeights.push_back(Synapse());
		m_inputWeights.back().weight = RandomWeight(rng, effectiveInputs, numOutputs);
	}
	std::cout << "N" << m_myType << " ";
}

void Neuron::CalcHiddenGradients(const Layer& nextLayer)
{
	if (IsAlive()) {
		float dow = SumDOW(nextLayer);
		if (m_myType != 99) {
			m_gradient = dow * Neuron::TransferFunctionDerivative(m_myType, m_inputVals);
		}
		else {
			//SoftMax
			m_gradient = dow * m_outputVal*(1.0f - m_outputVal);
		}
		HardClipping();
	}
}

void Neuron::CalcOutputGradients(float targetVal)
{
	if (m_neuronSet.GetNetLoss() == SettingManager::Loss::LogLoss && m_myType == 99) {
		//Cross Entropy Grad
		m_gradient = targetVal - m_outputVal;
	}
	else {
		//Mean Squared
		float delta = targetVal - m_outputVal;
		if (m_myType != 99) {
			m_gradient = delta * Neuron::TransferFunctionDerivative(m_myType, m_inputVals);
		}
		else {
			//SoftMax
			m_gradient = delta * m_outputVal*(1.0f - m_outputVal);
		}
	}
	HardClipping();
}

void Neuron::FeedForward(const Layer& prevLayer)
{
	if (IsAlive()) {
		float sum = 0.0f;

		m_prevState = m_outputVal;

		//-----------------------------------------
		//Feeding Forward
		//Dropout on, but Testing
		if (m_neuronSet.isDropoutActive() && !IsTraining()) {
			float dropMod = ((100.0f - m_neuronSet.GetDropout()) / 100.0f);
			for (unsigned n = 0; n < prevLayer.size(); ++n) {
				if (prevLayer[n].IsAlive()) {
					sum += prevLayer[n].GetOutputVal() * m_inputWeights[n].weight*dropMod;
				}
			}
			if (m_neuronSet.GetSimpleRecurrency()) {
				if (!m_neuronSet.isLinearRestricted() || (m_myType != 1 && m_myType != 4)) {
					sum += m_prevState * m_inputWeights.back().weight*dropMod;
				}
			}
		}
		//Normal
		else {
			for (unsigned n = 0; n < prevLayer.size(); ++n) {
				if (prevLayer[n].IsAlive()) {
					sum += prevLayer[n].GetOutputVal() * m_inputWeights[n].weight;
				}
			}
			if (m_neuronSet.GetSimpleRecurrency()) {
				if (!m_neuronSet.isLinearRestricted() || (m_myType != 1 && m_myType != 4)) {
					sum += m_prevState * m_inputWeights.back().weight;
				}
			}
		}
		//Added debug check for NANs, Alerts of issue and stops further training. 
		if (isnan(sum)) {
			std::cout << "Somethings's causing the issue before this point!";
			std::cin.get();
			for (unsigned n = 0; n < prevLayer.size(); ++n) {
				//std::cout << prevLayer[n].GetOutputVal() * m_inputWeights[n].weight << "\n";
				if (isnan(prevLayer[n].GetOutputVal() * m_inputWeights[n].weight)) {
					std::cout << "Output: " << prevLayer[n].GetOutputVal() << "\n";
					std::cout << "Weight: " << m_inputWeights[n].weight << "\n";
					std::cout << "Its ID: " << n << ", my ID is: " << m_myIndex << "\n";
					std::cout << "Its type: " << prevLayer[n].GetMyType() << ", my Type: " << m_myType << "\n";
					std::cout << "Its Memory: " << prevLayer[n].GetCellMemory() << "\n";
					std::cin.get();
				}
			}
			std::cin.get();
		}

		m_inputVals = sum;
		m_outputVal = Neuron::TransferFunction(m_myType, sum);
	}
}

void Neuron::FeedForwardSM(const Layer& prevLayer, const Layer& currentLayer)
{
	m_smInputs.clear();
	if (m_myIndex != 0) {
		m_inputVals = currentLayer[0].GetsmInputs(m_myIndex);
		m_smSum = currentLayer[0].GetsmSum();
		m_outputVal = m_inputVals / m_smSum;
	}
	else {
		float sum = 0.0f;
		float smMod = 0.0f;

		//----------------------------------------------------
		//Calculate SoftMax Mod
		for (unsigned clCount = 0; clCount < currentLayer.size() - 1; ++clCount) {
			sum = 0.0f;
			for (unsigned n = 0; n < prevLayer.size(); ++n) {
				if (prevLayer[n].IsAlive()) {
					sum += prevLayer[n].GetOutputVal() * currentLayer[clCount].m_inputWeights[n].weight;
				}
			}
			smMod = std::max(smMod, sum);
		}
		
		//----------------------------------------------------
		//Calculate Outputs
		m_smSum = 0.0f;
		//Dropout on, but now testing.
		if (m_neuronSet.isDropoutActive() && !IsTraining()) {
			float dropMod = ((100.0f - m_neuronSet.GetDropout()) / 100.0f);
			for (unsigned clCount = 0; clCount < currentLayer.size() - 1; ++clCount) {
				sum = 0.0f;
				for (unsigned n = 0; n < prevLayer.size(); ++n) {
					if (prevLayer[n].IsAlive()) {
						sum += prevLayer[n].GetOutputVal() * currentLayer[clCount].m_inputWeights[n].weight*dropMod;
					}
				}
				m_smInputs.push_back(exp(sum - smMod));
				m_smSum += exp(sum - smMod);
			}
		}
		//Normal
		else {
			for (unsigned clCount = 0; clCount < currentLayer.size() - 1; ++clCount) {
				sum = 0.0f;
				for (unsigned n = 0; n < prevLayer.size(); ++n) {
					if (prevLayer[n].IsAlive()) {
						sum += prevLayer[n].GetOutputVal() * currentLayer[clCount].m_inputWeights[n].weight;
					}
				}
				m_smInputs.push_back(exp(sum - smMod));
				m_smSum += exp(sum - smMod);
			}
		}
		m_inputVals = m_smInputs[m_myIndex];
		m_outputVal = m_inputVals / m_smSum;
	}
}

void Neuron::NormClipping(const float& inNorm)
{
	if (IsAlive()) {
		if (inNorm != 0.0f && inNorm > m_neuronSet.GetClipThreshold()) {
			m_gradient = m_gradient * (m_neuronSet.GetClipThreshold() / inNorm);
		}
	}
}

void Neuron::UpdateInputWeights(Layer& prevLayer)
{
	if (IsAlive()) {
		//Normal Update
		for (unsigned n = 0; n < prevLayer.size(); n++) {
			if (prevLayer[n].IsAlive()) {
				float oldDeltaWeight = m_inputWeights[n].deltaWeight;

				float newDeltaWeight =
					m_neuronSet.GetEta()*prevLayer[n].GetOutputVal()*m_gradient + m_neuronSet.GetAlpha()*oldDeltaWeight;

				m_inputWeights[n].deltaWeight = newDeltaWeight;
				m_inputWeights[n].weight += newDeltaWeight;
			}
		}
		//Update Recurrent Weight
		if (m_neuronSet.GetSimpleRecurrency() && m_myType != 99) {
			if (!m_neuronSet.isLinearRestricted() || (m_myType != 1 && m_myType != 4)) {
				float oldDeltaWeight = m_inputWeights.back().deltaWeight;

				float newDeltaWeight =
					m_neuronSet.GetEta()*m_prevState*m_gradient + m_neuronSet.GetAlpha()*oldDeltaWeight;

				m_inputWeights.back().deltaWeight = newDeltaWeight;
				m_inputWeights.back().weight += newDeltaWeight;
			}
		}
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
		if (nextLayer[n].IsAlive()) {
			sum += nextLayer[n].m_inputWeights[m_myIndex].weight*nextLayer[n].m_gradient;
		}
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
