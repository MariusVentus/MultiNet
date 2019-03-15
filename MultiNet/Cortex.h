#pragma once
#include "DataHandler.h"
#include "NeuralNet.h"
#include "RandHandler.h"
#include "SettingManager.h"
#include "Topology.h"
#include <string>


class Cortex {
public:
//Public Variables

//Public Functions
	Cortex(const std::string& settingFile, const std::string& topologyFile, const std::string& inputFile, const std::string& outputFile);
	void Train(void);
private:
//Private Objects
	SettingManager m_Settings;
	DataHandler m_Input;
	DataHandler m_Output;
	Topology m_Top;
	NeuralNet m_NN;
	RandHandler m_rng;

//Private Variables
//Private Functions
};