#pragma once
#include "SettingManager.h"
#include <string>
#include <vector>

class DataHandler {
//Variables
public:
	//Default Datafile Names/locations
	std::string inputFile = "Data\inputDefault.txt";
	std::string targetFile = "Data\outputDefault.txt";
	std::string singleIOFile = "Data\ioDefault.txt";

//Functions
public:
	void LoadBuffer(std::vector<std::vector<float>>& buffer, std::ifstream& dataStream, const unsigned bufferSize);

private:
	//Identifies "?" character, which denotes a missing value. This should be tossed. 
	bool MissingVals(const std::string& line);
	//Reads Line, clears out common issues with a text csv (white space, lines, extra carriage returns).
	void ReadLineAndClean(std::ifstream& dataStream, std::string& dataString);
	std::vector<float>SplitIntoFloatTokens(const std::string& dataString);

};