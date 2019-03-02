#pragma once
#include "SettingManager.h"
#include <fstream>
#include <string>
#include <vector>

class DataHandler {
//Variables
public:
	
private:
	std::ifstream inStream;
	std::vector<std::vector<float>> dataBuffer;

//Functions
public:
	DataHandler(const std::string& ioFile);

	unsigned GetBuffSize(void) const { return dataBuffer.size(); }
	unsigned GetRowSize(void) const { return dataBuffer[0].size(); }
	std::vector<float> GetRowX(unsigned inX) const { return dataBuffer[inX]; }

	void ReloadBuffer(void);

private:
	void LoadBuffer(std::vector<std::vector<float>>& buffer, std::ifstream& dataStream, const unsigned bufferSize);
	//Identifies "?" character, which denotes a missing value. This should be tossed. 
	bool MissingVals(const std::string& line);
	//Reads Line, clears out common issues with a text csv (white space, lines, extra carriage returns).
	void ReadLineAndClean(std::ifstream& dataStream, std::string& dataString);
	std::vector<float>SplitIntoFloatTokens(const std::string& dataString);

};