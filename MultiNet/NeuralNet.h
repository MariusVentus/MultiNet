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
	void OverwriteNeuralNet(const std::string& fileData);
	void FeedForward(const std::vector<float>& inputVals);
	void BackProp(const std::vector<float>& targetVals);
	void GetResults(std::vector<float>& resultVals) const;
	void ClearNetMemory(void);
	void FullRevive(void);
	void SetTraining(const bool& set);
	void RefreshError(const std::vector<float>& targetVals);

	float GetError(void) const { return m_error; }
	bool IsTraining(void) const { return m_isTraining; }

	unsigned GetLayerCount(void) const { return (unsigned)m_layers.size(); }
	unsigned GetNCount(unsigned layerNum) const { return (unsigned)m_layers[layerNum].size(); }
	unsigned GetNType(unsigned lNum, unsigned nNum) const { return (unsigned)m_layers[lNum][nNum].GetMyType(); }
	std::vector<float> GetNWeightVec(unsigned lNum, unsigned nNum) const { return m_layers[lNum][nNum].GetWeightVec(); }

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