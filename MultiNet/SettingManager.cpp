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
			m_Eta = std::stof(line);
		}
		else if (line == "[Alpha]") {
			ReadLineAndClean(inStream, line);
			m_Alpha = std::stof(line);
		}
		else if (line == "[Leak]") {
			ReadLineAndClean(inStream, line);
			m_Leak = std::stof(line);
		}
		else if (line == "[BufferSize]") {
			ReadLineAndClean(inStream, line);
			m_BufferSize = std::stoul(line);
		}
		else if (line == "[RandomizeTrainingData]") {
			ReadLineAndClean(inStream, line);
			m_RandomizedTraining = Stob(line);
		}
		else if (line == "[Initialization]") {
			ReadLineAndClean(inStream, line);
			if (line == "Normal") {
				m_NetI = Initialization::Normal;
			}
			else if (line == "XavierSimple") {
				m_NetI = Initialization::XavierSimple;
			}
			else if (line == "XavierFull") {
				m_NetI = Initialization::XavierFull;
			}
		}
		else if (line == "[GradientClipping]") {
			ReadLineAndClean(inStream, line);
			if (line == "None") {
				m_NetC = Clipping::None;
			}
			else if (line == "Hard") {
				m_NetC = Clipping::HardClip;
			}
			else if (line == "L2") {
				m_NetC = Clipping::L2Clip;
			}
			ReadLineAndClean(inStream, line);
			m_ClipThreshold = std::stof(line);
		}
		else if (line == "[ReservePercentage]") {
			ReadLineAndClean(inStream, line);
			m_ReservePercentage = std::stof(line);
			if (m_ReservePercentage > 50.0f) {
				m_ReservePercentage = 50.0f;
			}
			else if (m_ReservePercentage < 0.0f) {
				m_ReservePercentage = 0.0f;
			}
		}
		else if (line == "[NetLoss]") {
			ReadLineAndClean(inStream, line);
			if (line == "MeanSquared") {
				m_NetL = Loss::MeanSquared;
			}
			else if (line == "LogLoss") {
				m_NetL = Loss::LogLoss;
			}
		}
		else if (line == "[NetError]") {
			ReadLineAndClean(inStream, line);
			if (line == "MeanSquared") {
				m_NetE = Loss::MeanSquared;
			}
			else if (line == "LogLoss") {
				m_NetE = Loss::LogLoss;
			}
		}
		else if (line == "[SimpleRecurrency]") {
			ReadLineAndClean(inStream, line);
			m_SimpleRecurrency = Stob(line);
		}
		else if (line == "[RestrictLinearMemory]") {
			ReadLineAndClean(inStream, line);
			m_RestrictLinear = Stob(line);
		}
		else if (line == "[SmushedInputs]") {
			ReadLineAndClean(inStream, line);
			m_SmushedInputs = Stob(line);
			if (m_SmushedInputs == true) {
				ReadLineAndClean(inStream, line);
				if (line == "Auto" || line == "auto") {
					m_AutoSmush = true;
				}
				else {
					m_AutoSmush = false;
					m_SmushBy = std::stof(line);
					if (m_SmushBy == 0.0f) {
						m_SmushBy = 1.0f;
					}
				}
			}
		}
		else if (line == "[ExpandInput]") {
			ReadLineAndClean(inStream, line);
			m_ExpandIn = Stob(line);
		}
		else if (line == "[ExpandOutput]") {
			ReadLineAndClean(inStream, line);
			m_ExpandOut = Stob(line);
		}
		else if (line == "[MainDisplay]") {
			ReadLineAndClean(inStream, line);
			m_MainDisplay = Stob(line);
		}
		else if (line == "[VectorOutputtoNum]") {
			ReadLineAndClean(inStream, line);
			m_HotToNum = Stob(line);
		}
		else if (line == "[Dropout]") {
			ReadLineAndClean(inStream, line);
			m_IsDropout = Stob(line);
			ReadLineAndClean(inStream, line);
			m_DropoutPercentage = std::stof(line);
			if (m_DropoutPercentage > 100.0f) {
				m_DropoutPercentage = 100.0f;
			}
			else if (m_DropoutPercentage <= 0.0f) {
				m_DropoutPercentage = 1.0f;
			}
		}
		else if (line == "[PermanentMemory]") {
			ReadLineAndClean(inStream, line);
			m_PermMem = Stob(line);
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
