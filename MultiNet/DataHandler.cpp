#include "DataHandler.h"
#include <sstream>

DataHandler::DataHandler(const SettingManager& dhSet, const std::string & ioFile)
	:
	m_dhSet(dhSet),
	inStream(ioFile)
{
	m_EoF = !LoadBuffer(dataBuffer, inStream, m_dhSet.GetBufferSize());
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

bool DataHandler::LoadBuffer(std::vector<std::vector<float>>& buffer, std::ifstream & dataStream, const unsigned bufferSize)
{
	std::string dataTemp;

	for (unsigned nLines = 0; nLines < bufferSize; ++nLines) {
		dataTemp.clear();
		ReadLineAndClean(dataStream, dataTemp);
		if (dataTemp.empty()) {
			return false;
		}
		buffer.emplace_back(SplitIntoFloatTokens(dataTemp));
	}
	return true;
}

bool DataHandler::MissingVals(const std::string & line)
{
	if (line.find("?") != std::string::npos) {
		return true;
	}
	else {
		return false;
	}
}

void DataHandler::ReadLineAndClean(std::ifstream& dataStream, std::string & dataString)
{
	do {
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

std::vector<float> DataHandler::SplitIntoFloatTokens(const std::string & dataString)
{
	std::vector<float> result;

	std::stringstream lineStream(dataString);
	std::string	cell;

	while (std::getline(lineStream, cell, ',')) {
		result.emplace_back(stof(cell));
	}

	return result;
}
