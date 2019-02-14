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
