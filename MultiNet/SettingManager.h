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
	static constexpr unsigned bufferSize = 4;
	static constexpr bool SemiCasComma = true;

	//Initialization
	enum class Initialization { Normal, XavierSimple, XavierFull};
	static constexpr Initialization netI = Initialization::Normal;

	//Loss
	enum class Loss { MeanSquared, LogLoss };
	static constexpr Loss netL = Loss::MeanSquared;

private:
	//Learning
	float eta = .2f;
	float alpha = 0.5f;
	float leak = 0.01f; //For the Relu.

//Functions
public:
	SettingManager() = delete;
	SettingManager(const std::string& setFile);

	float GetEta(void) const { return eta; }
	float GetAlpha(void) const { return alpha; }
	float GetLeak(void) const { return leak; }

private:
	void InputSettings(const std::string& dataString);
	void ReadLineAndClean(std::ifstream& dataStream, std::string& dataString);

};