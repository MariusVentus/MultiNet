#pragma once
#include "NeuralNet.h"
#include <vector>

class SaveHandler {
public:
	SaveHandler(const NeuralNet& inNet);
	//SaveHandler(const std::string& saveFile);

private:
	std::string m_Network;

};