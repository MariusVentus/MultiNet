#pragma once
#include "Neuron.h"
#include "SettingManager.h"
#include "Topology.h"
#include <random>
#include <vector>

class NeuralNet {
	typedef std::vector<Neuron> Layer;

public:
	NeuralNet(const Topology& netTop, const SettingManager& netSet);
	void FeedForward(const std::vector<float>& inputVals);
	void BackProp(const std::vector<float>& targetVals);
	void GetResults(std::vector<float>& resultVals) const;
	void ClearNetMemory(void);
	void FullRevive(void);
	void SetTraining(const bool& set);

	float GetError(void) const { return m_error; }
	bool IsTraining(void) const { return m_isTraining; }

private:
//Vars
	std::vector<Layer>m_layers;
	float m_error = 0.0;
	SettingManager m_netSet;
	bool m_isTraining = false;

	// Local Random for Dropout
	std::random_device m_rd;
	std::mt19937 m_rng;
};