#pragma once
#include <vector>

class Topology {
private:
	/*
	Neuron Index > See Neuron.h
	Bias = 0
	Linear = 1
	Sigmoidal = 2
	Tanh = 3
	Leaky Relu = 4
	Gaussian = 5

	Softmax = 99
	*/

//Const
	const unsigned nTypeIndexLength = 5; //Non-Bias Neuron Types.
//Var
	typedef std::vector<unsigned> nTypes;
	std::vector<nTypes> layerTop;
//Func
public:
	Topology(const unsigned inLayerCount);
	void CreateMixedLayer(unsigned layerNum, unsigned nCount); //Only one Linear Line
	void CreateMonoLayer(unsigned layerNum, unsigned nCount, unsigned inType);
	void FillLayerTypes(unsigned layerNum, const std::vector<unsigned>& inTypes);

	unsigned GetLayerCount(void) const { return unsigned(layerTop.size()); }
	unsigned GetNCount(unsigned layerNum) const { return unsigned(layerTop[layerNum].size()); }
	unsigned GetNType(unsigned lNum, unsigned nNum) const { return layerTop[lNum][nNum]; }

};