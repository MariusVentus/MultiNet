#pragma once
#include "NeuralNet.h"
#include <vector>

class SaveHandler {
public:
	//Saving Constructor
	SaveHandler(const NeuralNet& inNet);
	//Loading Consructor
	SaveHandler(const std::string& saveFile);
	std::string GetNetString(void) const { return m_Network; }

	//Vars
	bool m_LoadSuccess = false;

private:
	std::string m_Network;

};