#include "DataHandler.h"
#include <fstream>
#include <sstream>

void DataHandler::LoadBuffer(std::vector<std::vector<float>>& buffer, std::ifstream & dataStream, const unsigned bufferSize)
{
	std::string dataTemp;
	std::vector<float> dataVec;

	for (unsigned nLines = 0; nLines < bufferSize; ++nLines) {
		dataTemp.clear();
		ReadLineAndClean(dataStream, dataTemp);
		if (dataTemp.empty()) {
			break;
		}
		buffer.emplace_back(SplitIntoFloatTokens(dataTemp));
	}
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

void DataHandler::ReadLineAndClean(std::ifstream & dataStream, std::string & dataString)
{
	do {
		do {
			std::getline(dataStream, dataString);
		} while (MissingVals(dataString));


		while (dataString.find(" ") != std::string::npos) {
			dataString.erase(dataString.find(" "), 1);
		}
		while (dataString.find("\t") != std::string::npos) {
			dataString.erase(dataString.find("\t"), 1);
		}
		while (dataString.find(",,") != std::string::npos) {
			dataString.erase(dataString.find(",,"), 1);
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
