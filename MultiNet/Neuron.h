#pragma once
#include "SettingManager.h"
#include <vector>

/*
Neuron Index > See Topology.h
Bias = 0
Linear = 1
Sigmoidal = 2
Tanh = 3
Leaky Relu = 4
Gaussian = 5
*/

class Neuron {
	typedef std::vector<Neuron> Layer;

public:
//Objects
	struct Synapse {
		float weight;
		float deltaWeight;
	};
//Public Functions
	Neuron(unsigned numInputs, unsigned myIndex, unsigned myType);
	void CalcHiddenGradients(const Layer& nextLayer);
	void CalcOutputGradients(float targetVal);
	void FeedForward(const Layer& prevLayer);
	void UpdateInputWeights(Layer& prevLayer);

	float GetGradients(void) const { return m_gradient; }
	unsigned GetMyType(void) const { return m_myType; }
	float GetOutputVal(void) const { return m_outputVal; }
	void SetOutputVal(float val) { m_outputVal = val; }

private:
	SettingManager Settings; //No init yet since everything currently static. 
//Variables
	std::vector<Synapse> m_outputWeights;
	float m_gradient;
	unsigned m_myIndex;
	unsigned m_myType;
	float m_outputVal = 0.0f;
	float m_inputVals = 0.0f;

//Private Functions
	float SumDOW(const Layer& nextLayer); //Sum of Derivatives of Weights.

	//Randomization
	static float RandomWeightStandard(void);

	//Transfer Functions
	static float TransferFunction(unsigned inType, float x);
	float TransferFunctionDerivative(unsigned inType, float x);

};