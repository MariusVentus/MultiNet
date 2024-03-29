#pragma once
#include "SettingManager.h"
#include <random>
#include <vector>
#include <assert.h>

/*
Neuron Index > See Topology.h
Bias = 0
Linear = 1
Sigmoidal = 2
Tanh = 3
Leaky Relu = 4
Gaussian = 5

Sine = 50

SoftMax = 99
*/

class Neuron {
	typedef std::vector<Neuron> Layer;

public:
//Objects
	struct Synapse {
		float weight = 0.0f;
		float deltaWeight = 0.0f;
	};
//Public Functions
	Neuron(const SettingManager& nSet, unsigned numInputs, unsigned numOutputs, unsigned myIndex, unsigned myType);
	void CalcHiddenGradients(const Layer& nextLayer);
	void CalcOutputGradients(float targetVal);
	void FeedForward(const Layer& prevLayer);
	void FeedForwardSM(const Layer& prevLayer, const Layer& currentLayer);
	void NormClipping(const float& inNorm);
	void UpdateInputWeights(Layer& prevLayer);

	float GetGradients(void) const { return m_gradient; }
	unsigned GetMyType(void) const { return m_myType; }
	float GetOutputVal(void) const { return m_outputVal; }
	void SetOutputVal(float val) { m_outputVal = val; }
	float GetsmSum(void) const { return m_smSum; }
	float GetsmInputs(unsigned indexNum) const { return m_smInputs[indexNum]; }
	void ClearCellMemory(void) {
		assert(!m_neuronSet.IsMemPerm());
		m_prevState = 0.0f; }
	float GetCellMemory(void) const { return m_prevState; }
	std::vector<float> GetWeightVec(void) const;

	bool IsAlive(void) const { return m_isAlive; }
	void KillNeuron(void) { m_isAlive = false; }
	void ReviveNeuron(void) { m_isAlive = true; }
	bool IsTraining(void) const { return m_isTraining; }
	void SetTraining(const bool& set) { m_isTraining = set; }

	void OverwriteWeights(const std::vector<float>& inVec);

private:
//Variables
	SettingManager m_neuronSet;
	std::vector<Synapse> m_inputWeights;
	std::vector<float> m_smInputs;
	unsigned m_myType;
	unsigned m_myIndex;
	float m_gradient = 0.0f;
	float m_inputVals = 0.0f;
	float m_outputVal = 0.0f;
	float m_smSum = 0.0f;
	float m_prevState = 0.0f;
	bool m_isAlive = true;
	bool m_isTraining = false;


//Private Functions
	void HardClipping(void);
	float SumDOW(const Layer& nextLayer) const; //Sum of Derivatives of Weights.

	//Randomization
	static float RandomWeightStandard(void) { return 4.0f; } //Rolled a Die 
	float RandomWeight(std::mt19937& rng, const unsigned& numInputs, const unsigned& numOutputs);

	//Transfer Functions
	float TransferFunction(unsigned inType, float x);
	float TransferFunctionDerivative(unsigned inType, float x);

};