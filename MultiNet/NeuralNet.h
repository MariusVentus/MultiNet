#pragma once
#include "Neuron.h"
#include "Topology.h"
#include <vector>

class NeuralNet {
	typedef std::vector<Neuron> Layer;

public:
	NeuralNet(const Topology& netTop);
	void FeedForward(const std::vector<float>& inputVals);
	void BackProp(const std::vector<float>& targetVals);
	void GetResults(std::vector<float>& resultVals) const;

	float GetError(void) const { return m_error; }

private:
//Vars
	std::vector<Layer>m_layers;
	float m_error = 0.0;
};