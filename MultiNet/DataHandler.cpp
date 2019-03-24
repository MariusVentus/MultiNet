#include "DataHandler.h"
#include <algorithm>
#include <cmath>
#include <sstream>

DataHandler::DataHandler(const SettingManager& dhSet, const std::string & ioFile)
	:
	m_dhSet(dhSet),
	inStream(ioFile),
	m_maxInputCount(MaxInputCount()),
	m_TrainingDataSize(static_cast<unsigned>(std::round(m_maxInputCount*((100.0f - m_dhSet.GetReservePercentage()) / 100.0f))))
{
	PopulateMaxValArr();
	m_EoF = !LoadBuffer(dataBuffer, inStream, m_dhSet.GetBufferSize());
}

unsigned DataHandler::GetMaxArrLoc(const std::vector<float>& inArr)
{
	return static_cast<unsigned>(std::distance(inArr.begin(), std::max_element(inArr.begin(), inArr.end())));
}

std::vector<float> DataHandler::GetExpandedRowX(unsigned inX) const
{
	if (dataBuffer[0].size() == 1) {
		std::vector<float> tempBuff;
		unsigned columnNum = static_cast<unsigned>(dataBuffer[inX][0]);
		for (unsigned i = 0; i <= m_MaxValArr[0]; i++) {
			if (columnNum != i) {
				tempBuff.emplace_back(0.0f);
			}
			else {
				tempBuff.emplace_back(1.0f);
			}
		}
		return tempBuff;
	}
	else {
		return dataBuffer[inX];
	}
}

unsigned DataHandler::GetRowSize(void) const
{
	if (m_dhSet.GetExpandedCol() && dataBuffer[0].size() == 1) {
		return static_cast<unsigned>(GetExpandedRowX(0).size());
	}
	else {
		return static_cast<unsigned>(dataBuffer[0].size());
	}
}

std::vector<float> DataHandler::GetSmushedRowX(unsigned inX) const
{
	std::vector<float> tempBuff(dataBuffer[inX].size());
	for (unsigned i = 0; i < dataBuffer[inX].size(); i++) {
		tempBuff[i] = dataBuffer[inX][i] / m_MaxValArr[i];
	}
	return tempBuff;
}

bool DataHandler::LoadTestBuff(void)
{
	std::string dataTemp;
	
	for (unsigned nLines = 0; nLines < m_dhSet.GetBufferSize(); ++nLines) {
		dataTemp.clear();
		ReadLineAndClean(inStream, dataTemp);
		if (dataTemp.empty()) {
			return false;
		}
		dataBuffer.emplace_back(SplitIntoFloatTokens(dataTemp));
	}
	return true;
}

void DataHandler::PrepTest(void)
{
	std::string dataTemp;

	ResetDh();

	for (; m_LoadedCount < m_TrainingDataSize; ++m_LoadedCount) {
		ReadLineAndClean(inStream, dataTemp);
	}
}

void DataHandler::ReloadBuffer(void)
{
	dataBuffer.clear();
	m_EoF = !LoadBuffer(dataBuffer, inStream, m_dhSet.GetBufferSize());

	if(m_EoF == true && dataBuffer.empty()){
		m_EoFDelayed = m_EoF;
		inStream.clear();
		inStream.seekg(0, inStream.beg);
		m_EoF = !LoadBuffer(dataBuffer, inStream, m_dhSet.GetBufferSize());
	}
}

void DataHandler::ReloadTestBuffer(void)
{
	dataBuffer.clear();
	m_EoF = !LoadTestBuff();

	if (m_EoF == true && dataBuffer.empty()) {
		m_EoFDelayed = m_EoF;
	}
}

void DataHandler::ResetDh(void)
{
	dataBuffer.clear();
	inStream.clear();
	inStream.seekg(0, inStream.beg);
	ResetLoadedCount();
	m_EoF = false;
	ResetEoF();
}

bool DataHandler::LoadBuffer(std::vector<std::vector<float>>& buffer, std::ifstream & dataStream, const unsigned bufferSize)
{
	std::string dataTemp;

	for (unsigned nLines = 0; nLines < bufferSize && m_LoadedCount < m_TrainingDataSize; ++nLines, ++m_LoadedCount) {
		dataTemp.clear();
		ReadLineAndClean(dataStream, dataTemp);
		if (dataTemp.empty()) {
			return false;
		}
		buffer.emplace_back(SplitIntoFloatTokens(dataTemp));
	}
	if (m_LoadedCount >= m_TrainingDataSize) {
		return false;
	}
	return true;
}

unsigned DataHandler::MaxInputCount(void)
{
	unsigned maxInputCount = 0;
	std::string dataTemp;

	inStream.clear();
	inStream.seekg(0, inStream.beg);
	do {
		dataTemp.clear();
		ReadLineAndClean(inStream, dataTemp);
		if (!dataTemp.empty()) {
			maxInputCount++;
		}
	} while (!dataTemp.empty());
	inStream.clear();
	inStream.seekg(0, inStream.beg);

	return maxInputCount;
}

void DataHandler::PopulateMaxValArr(void)
{
	std::string dataTemp;
	std::vector<float> valArr;
	inStream.clear();
	inStream.seekg(0, inStream.beg);

	ReadLineAndClean(inStream, dataTemp);
	m_MaxValArr = SplitIntoFloatTokens(dataTemp);

	do {
		dataTemp.clear();
		ReadLineAndClean(inStream, dataTemp);
		if (!dataTemp.empty()) {
			valArr = SplitIntoFloatTokens(dataTemp);
			for (unsigned i = 0; i < m_MaxValArr.size(); i++) {
				m_MaxValArr[i] = std::max(m_MaxValArr[i], valArr[i]);
			}
		}
	} while (!dataTemp.empty());

	inStream.clear();
	inStream.seekg(0, inStream.beg);

}

void DataHandler::ReadLineAndClean(std::ifstream& dataStream, std::string & dataString)
{
	do {
		std::getline(dataStream, dataString);

		//SemiColons to Comma
		while (dataString.find(";") != std::string::npos) {
			dataString.replace(dataString.find(";"), 1, ",");
		}
		//Remove White Space
		while (dataString.find(" ") != std::string::npos) {
			dataString.erase(dataString.find(" "), 1);
		}
		while (dataString.find("\t") != std::string::npos) {
			dataString.erase(dataString.find("\t"), 1);
		}
		//Remove Double Commas
		while (dataString.find(",,") != std::string::npos) {
			dataString.erase(dataString.find(",,"), 1);
		}
		//Remove Leading Comma
		if (dataString.find(",") == 0) {
			dataString.erase(dataString.find(","), 1);
		}
		//Remove Trailing Comma
		if (dataString.find_last_of(",") == dataString.size() - 1 && !dataString.empty()) {
			dataString.pop_back();
		}
	} while (!dataStream.eof() && dataString.empty());
}

std::vector<float> DataHandler::SplitIntoFloatTokens(const std::string& dataString)
{
	std::vector<float> result;

	std::stringstream lineStream(dataString);
	std::string	cell;

	while (std::getline(lineStream, cell, ',')) {
		result.emplace_back(stof(cell));
	}

	return result;
}
