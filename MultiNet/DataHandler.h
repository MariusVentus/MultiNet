#pragma once
#include "SettingManager.h"
#include <fstream>
#include <string>
#include <vector>

class DataHandler {
//Variables
public:
	
private:
	bool m_EoF = false;
	bool m_EoFDelayed = false;
	std::ifstream inStream;
	std::vector<std::vector<float>> dataBuffer;
	SettingManager m_dhSet;
	unsigned m_LoadedCount = 0;
	unsigned m_maxInputCount;
	unsigned m_TrainingDataSize;


//Functions
public:
	DataHandler() = delete;
	DataHandler(const SettingManager& dhSet, const std::string& ioFile);

	unsigned GetBuffSize(void) const { return dataBuffer.size(); }
	bool GetEoF(void) const { return m_EoFDelayed; }
	unsigned GetMaxInputs(void) const { return m_maxInputCount; }
	unsigned GetRowSize(void) const { return dataBuffer[0].size(); }
	std::vector<float> GetRowX(unsigned inX) const { return dataBuffer[inX]; }
	unsigned GetTrainingDataSize(void) const { return m_TrainingDataSize; }

	void ReloadBuffer(void);
	void ResetEoF(void) { m_EoFDelayed = false; }
	void ResetLoadedCount(void) { m_LoadedCount = 0; }

private:
	//Returns true if it can continue, false if at end of file. 
	bool LoadBuffer(std::vector<std::vector<float>>& buffer, std::ifstream& dataStream, const unsigned bufferSize);
	//Checks File, Counts Maximum inputs that can load into buffer. 
	unsigned MaxInputCount(void);
	//Reads Line, clears out common issues with a text csv (white space, lines, extra carriage returns).
	void ReadLineAndClean(std::ifstream& dataStream, std::string& dataString);
	std::vector<float>SplitIntoFloatTokens(const std::string& dataString);

};