#include "SettingManager.h"
#include <fstream>


SettingManager::SettingManager(const std::string& setFile)
{
	InputSettings(setFile);
}

void SettingManager::InputSettings(const std::string& dataString)
{
	std::ifstream inStream(dataString);
	std::string line;

	do {
		line.clear();
		ReadLineAndClean(inStream, line);
		if (line == "[Eta]") {
			ReadLineAndClean(inStream, line);
			eta = std::stof(line);
		}
		else if (line == "[Alpha]") {
			ReadLineAndClean(inStream, line);
			alpha = std::stof(line);
		}
		else if (line == "[Leak]") {
			ReadLineAndClean(inStream, line);
			leak = std::stof(line);
		}
		else if (line == "[BufferSize]") {
			ReadLineAndClean(inStream, line);
			bufferSize = std::stoul(line);
		}
		else if (line == "[RandomizeTrainingData]") {
			ReadLineAndClean(inStream, line);
			randomizedTraining = Stob(line);
		}
		else if (line == "[Initialization]") {
			ReadLineAndClean(inStream, line);
			if (line == "Normal") {
				netI = Initialization::Normal;
			}
			else if (line == "XavierSimple") {
				netI = Initialization::XavierSimple;
			}
			else if (line == "XavierFull") {
				netI = Initialization::XavierFull;
			}
		}
		else {
			continue;
		}

	} while (!line.empty());

}

void SettingManager::ReadLineAndClean(std::ifstream& dataStream, std::string& dataString)
{
	do {
		std::getline(dataStream, dataString);
		//Remove Comments
		if (dataString.find(";;") != std::string::npos) {
			dataString.erase(dataString.find(";;"));
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

bool SettingManager::Stob(const std::string& dataString) const
{
	if (dataString == "true" || dataString == "True") {
		return true;
	}
	else if (dataString == "false" || dataString == "False") {
		return false;
	}
	else {
		return false;
	}
}