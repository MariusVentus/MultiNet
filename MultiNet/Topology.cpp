#include "Topology.h"
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>

Topology::Topology(const std::string& setTop, const unsigned& inputSize, const unsigned& outputSize)
{
	std::ifstream inTop(setTop);
	InputTop(inTop, inputSize, outputSize);

}

void Topology::CreateMixedLayer(unsigned layerNum, unsigned nCount)
{
	std::vector<unsigned> tmpTypeLayer;
	unsigned tmpTotal = 0;
	unsigned i = 0;
	if (nCount != 0) {
		tmpTypeLayer.push_back(1);
	}
	while (i < nCount - 1) {
		for (unsigned nTypeTmp = 2; nTypeTmp <= nTypeIndexLength && tmpTotal < (nCount - 1); ++nTypeTmp) {
			tmpTypeLayer.push_back(nTypeTmp);
			++i;
			++tmpTotal;
		}
	}
	FillLayerTypes(layerNum, tmpTypeLayer);
}

void Topology::CreateMonoLayer(unsigned layerNum, unsigned nCount, unsigned inType)
{
	std::vector<unsigned> tmpTypeLayer;
	for (unsigned i = 0; i < nCount; ++i) {
		tmpTypeLayer.push_back(inType);
	}
	FillLayerTypes(layerNum, tmpTypeLayer);
}

void Topology::FillLayerTypes(unsigned layerNum, const std::vector<unsigned>& inTypes)
{
	for (unsigned typeNum = 0; typeNum < inTypes.size(); ++typeNum) {
		layerTop[layerNum].push_back(inTypes[typeNum]);
	}
}

void Topology::InputTop(std::ifstream& inTop, const unsigned& nInput, const unsigned& nOutput)
{
	std::string line;
	unsigned currentLayer = 0;
	unsigned count = 0;
	unsigned type = 0;
	unsigned repeat = 0; 
	do {
		line.clear();
		ReadLineAndClean(inTop, line);
		//Reading Layers
		if (line == "[Input]") {
			layerTop.push_back(nTypes());

			line.clear();
			ReadLineAndClean(inTop, line);
			std::stringstream layerInfo(line);
			std::string cell;
			//Type
			std::getline(layerInfo, cell, ',');
			type = TypeCheck(cell);
			//Count
			std::getline(layerInfo, cell, ',');
			if (cell.find("Auto") != std::string::npos || cell.find("auto") != std::string::npos) {
				count = nInput;
			}
			else {
				count = std::stoi(cell);
			}
			//Repeat
			std::getline(layerInfo, cell);
			if (!cell.empty()) {
				repeat = std::stoi(cell);
			}
			else {
				repeat = 0;
			}
			//Fill
			for (unsigned i = 0; i <= repeat; i++) {
				CreateMonoLayer(i, count, type);
				if (repeat > 0 && i != repeat) {
					layerTop.push_back(nTypes());
					currentLayer++;
				}
			}
		}
		else if (line == "[Mono]") {
			layerTop.push_back(nTypes());
			currentLayer++;

			line.clear();
			ReadLineAndClean(inTop, line);
			std::stringstream layerInfo(line);
			std::string cell;
			//Type
			std::getline(layerInfo, cell, ',');
			type = TypeCheck(cell);
			//Count
			std::getline(layerInfo, cell, ',');
			count = std::stoi(cell);
			//Repeat
			std::getline(layerInfo, cell);
			if (!cell.empty()) {
				repeat = std::stoi(cell);
			}
			else {
				repeat = 0;
			}
			//Fill
			unsigned startLayer = currentLayer;
			for (unsigned i = 0; i <= repeat; i++) {
				CreateMonoLayer(startLayer+i, count, type);
				if (repeat > 0 && i != repeat) {
					layerTop.push_back(nTypes());
					currentLayer++;
				}
			}
		}
		else if (line == "[Mixed]") {
			layerTop.push_back(nTypes());
			currentLayer++;

			line.clear();
			ReadLineAndClean(inTop, line);
			std::stringstream layerInfo(line);
			std::string cell;
			//Count
			std::getline(layerInfo, cell, ',');
			count = std::stoi(cell);
			//Repeat
			std::getline(layerInfo, cell);
			if (!cell.empty()) {
				repeat = std::stoi(cell);
			}
			else {
				repeat = 0;
			}
			//Fill
			unsigned startLayer = currentLayer;
			for (unsigned i = 0; i <= repeat; i++) {
				CreateMixedLayer(startLayer + i, count);
				if (repeat > 0 && i != repeat) {
					layerTop.push_back(nTypes());
					currentLayer++;
				}
			}
		}
		else if (line == "[Custom]") {
			layerTop.push_back(nTypes());
			currentLayer++;

			line.clear();
			ReadLineAndClean(inTop, line);
			std::stringstream layerInfo(line);
			unsigned commaCount = static_cast<unsigned>(std::count(line.begin(), line.end(), ','));
			unsigned loadRepeat = 0;
			std::string cell;
			std::vector<unsigned> tmpLayer;
			if (commaCount % 2 == 0) {
				loadRepeat = commaCount / 2;
			}
			else {
				loadRepeat = (commaCount - 1) / 2;
			}
			for (unsigned i = 0; i < loadRepeat; i++) {
				//Type
				std::getline(layerInfo, cell, ',');
				type = TypeCheck(cell);
				//Count
				std::getline(layerInfo, cell, ',');
				count = std::stoi(cell);
				for (unsigned j = 0; j < count; j++) {
					tmpLayer.push_back(type);
				}
			}
			//Repeat
			if (commaCount % 2 == 0) {
				std::getline(layerInfo, cell);
				if (!cell.empty()) {
					repeat = std::stoi(cell);
				}
				else {
					repeat = 0;
				}
			}
			//Fill
			unsigned startLayer = currentLayer;
			for (unsigned i = 0; i <= repeat; i++) {
				FillLayerTypes(startLayer + i, tmpLayer);
				if (repeat > 0 && i != repeat) {
					layerTop.push_back(nTypes());
					currentLayer++;
				}
			}
		}
		else if ( line == "[Output]") {
			layerTop.push_back(nTypes());
			currentLayer++;

			line.clear();
			ReadLineAndClean(inTop, line);
			std::stringstream layerInfo(line);
			std::string cell;
			//Type
			std::getline(layerInfo, cell, ',');
			type = TypeCheck(cell);
			//Count
			std::getline(layerInfo, cell, ',');
			if (cell.find("Auto") != std::string::npos || cell.find("auto") != std::string::npos) {
				count = nOutput;
			}
			else {
				count = std::stoi(cell);
			}
			//Repeat
			std::getline(layerInfo, cell);
			if (!cell.empty()) {
				repeat = std::stoi(cell);
			}
			else {
				repeat = 0;
			}
			//Fill
			unsigned startLayer = currentLayer;
			for (unsigned i = 0; i <= repeat; i++) {
				CreateMonoLayer(startLayer + i, count, type);
				if (repeat > 0 && i != repeat) {
					layerTop.push_back(nTypes());
					currentLayer++;
				}
			}
		}
		
	} while (!line.empty());
}

void Topology::ReadLineAndClean(std::ifstream& dataStream, std::string& dataString)
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

unsigned Topology::TypeCheck(const std::string & inType)
{
	if (inType == "Bias") {
		return 0;
	}
	else if (inType == "Linear") {
		return 1;
	}
	else if (inType == "Sigmoid") {
		return 2;
	}
	else if (inType == "Tanh") {
		return 3;
	}
	else if (inType == "Relu") {
		return 4;
	}
	else if (inType == "Gaussian") {
		return 5;
	}
	else if (inType == "Sine") {
		return 50;
	}
	else if (inType == "SoftMax") {
		return 99;
	}
	else {
		return 0;
	}
}
