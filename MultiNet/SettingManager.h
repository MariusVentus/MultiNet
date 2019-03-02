#pragma once
#include <string>

class SettingManager {
public:
//Placing these as temporary settings until a Reader is setup, and planning for features. 

	//Clipping
	enum class Clipping { None, HardClip, L2Clip };
	static constexpr Clipping netC = Clipping::None;
	static constexpr float clipThreshold = 1.0f;

	//Data
	static constexpr unsigned bufferSize = 50; 
	std::string inputFile = "Data\\inputDefault.txt";
	std::string targetFile = "Data\\outputDefault.txt";
	std::string singleIOFile = "Data\\ioDefault.txt";

	//Initialization
	enum class Initialization { Normal, XavierSimple, XavierFull};
	static constexpr Initialization netI = Initialization::Normal;

	//Learning
	static constexpr float eta = .2f;
	static constexpr float alpha = 0.5f;
	static constexpr float leak = 0.01f; //For the Relu.

	//Loss
	enum class Loss { MeanSquared, LogLoss };
	static constexpr Loss netL = Loss::MeanSquared;

};