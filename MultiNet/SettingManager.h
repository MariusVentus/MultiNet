#pragma once
#include <string>

class SettingManager {
public:
//Placing these as temporary settings until a Reader is setup, and planning for features. 

	//Clipping
	enum class Clipping { None, HardClip, L2Clip };
	static constexpr Clipping netC = Clipping::None;
	static constexpr float clipThreshold = 1.0f;

	//Initialization
	enum class Initialization { Normal, XavierSimple, XavierFull};
	static constexpr Initialization netI = Initialization::Normal;

	//Loss
	enum class Loss { MeanSquared, LogLoss };
	static constexpr Loss netL = Loss::MeanSquared;

private:
	//Data
	unsigned bufferSize;
	bool SemiCasComma;

	//Learning
	float eta;
	float alpha;
	float leak; //For the Relu.

//Functions
public:
	SettingManager() = delete;
	SettingManager(const std::string& setFile);

	float GetAlpha(void) const { return alpha; }
	unsigned GetBufferSize(void) const { return bufferSize; }
	float GetEta(void) const { return eta; }
	float GetLeak(void) const { return leak; }
	bool GetSemiCasComma(void) const { return SemiCasComma; }

private:
	void InputSettings(const std::string& dataString);
	void ReadLineAndClean(std::ifstream& dataStream, std::string& dataString);
	bool Stob(const std::string& dataString) const;

};