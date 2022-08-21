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
	void ManualRun(const std::string& manIn);
	void Train(const unsigned& epochMax);
	void Test(void);
	void Save(void);

	unsigned GetTrainingDataSize(void) const { return m_TrainingDataSize; }
private:
//Private Objects
	SettingManager m_Settings;
	DataHandler m_Input;
	DataHandler m_Output;
	Topology m_Top;
	NeuralNet m_NN;
	RandHandler m_rng;

//Private Variables
	unsigned m_TrainingDataSize;
	unsigned m_InputCount;
//Private Functions
	void DisplayTesting(unsigned buffRow, const std::vector<float>& inResultVals) const;
	void DisplayTraining(unsigned buffRow, const std::vector<float>& inResultVals) const;
	void DisplayTime(const std::string& timeType, const float& inTime) const;
	bool CheckAccuracy(const std::vector<float>& output, const std::vector<float>& target) const;
};