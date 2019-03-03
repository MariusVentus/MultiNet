#include "SettingManager.h"
#include <fstream>


SettingManager::SettingManager(const std::string& setFile)
{
	InputSettings(setFile);
}

void SettingManager::ReadLineAndClean(std::ifstream& dataStream, std::string& dataString)
{
	do {
		std::getline(dataStream, dataString);

		//Remove Comments
		if (dataString.find(";;") != std::string::npos) {
			dataString.erase(dataString.find(";;"));
		}

		//Remove White Space, Empty New Lines, Leading and Double Commas.
		while (dataString.find(" ") != std::string::npos) {
			dataString.erase(dataString.find(" "), 1);
		}
		while (dataString.find("\t") != std::string::npos) {
			dataString.erase(dataString.find("\t"), 1);
		}
		while (dataString.find(",,") != std::string::npos) {
			dataString.erase(dataString.find(",,"), 1);
		}
		while (dataString.find(",") == 0) {
			dataString.erase(dataString.find(","), 1);
		}
	} while (!dataStream.eof() && dataString.empty());
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
		else {
			continue;
		}

	} while (!line.empty());

}
