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
		else if (line == "[GradientClipping]") {
			ReadLineAndClean(inStream, line);
			if (line == "None") {
				netC = Clipping::None;
			}
			else if (line == "Hard") {
				netC = Clipping::HardClip;
			}
			else if (line == "L2") {
				netC = Clipping::L2Clip;
			}
			ReadLineAndClean(inStream, line);
			clipThreshold = std::stof(line);
		}
		else if (line == "[ReservePercentage]") {
			ReadLineAndClean(inStream, line);
			reservePercentage = std::stof(line);
			if (reservePercentage > 50.0f) {
				reservePercentage = 50.0f;
			}
			else if (reservePercentage < 0.0f) {
				reservePercentage = 0.0f;
			}
		}
		else if (line == "[NetLoss]") {
			ReadLineAndClean(inStream, line);
			if (line == "MeanSquared") {
				netL = Loss::MeanSquared;
			}
			else if (line == "LogLoss") {
				netL = Loss::LogLoss;
			}
		}
		else if (line == "[NetError]") {
			ReadLineAndClean(inStream, line);
			if (line == "MeanSquared") {
				netE = Loss::MeanSquared;
			}
			else if (line == "LogLoss") {
				netE = Loss::LogLoss;
			}
		}
		else if (line == "[SimpleRecurrency]") {
			ReadLineAndClean(inStream, line);
			simpleRecurrency = Stob(line);
		}
		else if (line == "[SmushedInputs]") {
			ReadLineAndClean(inStream, line);
			smushedInputs = Stob(line);
		}
		else if (line == "[ExpandSoloColumn]") {
			ReadLineAndClean(inStream, line);
			expandedCol = Stob(line);
		}
		else if (line == "[MainDisplay]") {
			ReadLineAndClean(inStream, line);
			mainDisplay = Stob(line);
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
