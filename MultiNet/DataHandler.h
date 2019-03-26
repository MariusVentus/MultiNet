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
	SettingManager m_dhSet;
	std::ifstream inStream;
	std::vector<std::vector<float>> dataBuffer;
	std::vector<float> m_MaxValArr;
	unsigned m_LoadedCount = 0;
	const unsigned m_maxInputCount;
	const unsigned m_TrainingDataSize;


//Functions
public:
	DataHandler() = delete;
	DataHandler(const SettingManager& dhSet, const std::string& ioFile);

	unsigned GetBuffSize(void) const { return static_cast<unsigned>(dataBuffer.size()); }
	bool GetEoF(void) const { return m_EoFDelayed; }
	unsigned GetMaxInputs(void) const { return m_maxInputCount; }
	std::vector<float> GetRowX(unsigned inX) const { return dataBuffer[inX]; }
	unsigned GetTrainingDataSize(void) const { return m_TrainingDataSize; }

	static unsigned GetMaxArrLoc(const std::vector<float>& inArr);
	std::vector<float> GetExpandedRowX(unsigned inX) const;
	unsigned GetIRowSize(void) const;
	unsigned GetORowSize(void) const;
	std::vector<float> GetSmushedRowX(unsigned inX) const;



	bool LoadTestBuff(void);
	void PrepTest(void);
	void ReloadBuffer(void);
	void ReloadTestBuffer(void);
	void ResetDh (void);
	void ResetEoF(void) { m_EoFDelayed = false; }
	void ResetLoadedCount(void) { m_LoadedCount = 0; }

private:
	//Returns true if it can continue, false if at end of file. 
	bool LoadBuffer(std::vector<std::vector<float>>& buffer, std::ifstream& dataStream, const unsigned bufferSize);
	//Checks File, Counts Maximum inputs that can load into buffer. 
	unsigned MaxInputCount(void);
	//Checks through the whole file, finds the largest number in each row. 
	void PopulateMaxValArr(void);
	//Reads Line, clears out common issues with a text csv (white space, lines, extra carriage returns).
	void ReadLineAndClean(std::ifstream& dataStream, std::string& dataString);
	//Split a string into float tokens.
	std::vector<float>SplitIntoFloatTokens(const std::string& dataString);

};