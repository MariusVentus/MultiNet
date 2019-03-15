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

//Functions
public:
	DataHandler() = delete;
	DataHandler(const SettingManager& dhSet, const std::string& ioFile);

	unsigned GetBuffSize(void) const { return dataBuffer.size(); }
	bool GetEoF(void) const { return m_EoFDelayed; }
	unsigned GetRowSize(void) const { return dataBuffer[0].size(); }
	std::vector<float> GetRowX(unsigned inX) const { return dataBuffer[inX]; }

	void ReloadBuffer(void);
	void ResetEoF(void) { m_EoFDelayed = false; }

private:
	//Returns true if it can continue, false if at end of file. 
	bool LoadBuffer(std::vector<std::vector<float>>& buffer, std::ifstream& dataStream, const unsigned bufferSize);
	//Reads Line, clears out common issues with a text csv (white space, lines, extra carriage returns).
	void ReadLineAndClean(std::ifstream& dataStream, std::string& dataString);
	std::vector<float>SplitIntoFloatTokens(const std::string& dataString);

};