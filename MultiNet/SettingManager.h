#pragma once
#include <string>

class SettingManager {
//Variables
public:
	//Clipping
	enum class Clipping { None, HardClip, L2Clip };

	//Initialization
	enum class Initialization { Normal, XavierSimple, XavierFull};

	//Loss
	enum class Loss { MeanSquared, LogLoss };

private:
	//Data
	unsigned bufferSize;
	bool randomizedTraining;
	float reservePercentage;
	bool smushedInputs;
	bool autoSmush;
	float smushBy;
	bool expandIn;
	bool expandOut;

	//Display
	bool mainDisplay;
	bool hotToNum;

	//Gradient Clipping
	Clipping netC = Clipping::None;
	float clipThreshold;

	//Initialization
	Initialization netI;

	//Learning
	float eta;
	float alpha;
	float leak; //For the Relu.

	//Loss (for Optimization) and Error (for reporting).
	Loss netL;
	Loss netE;

	//Simple Recurrency
	bool simpleRecurrency; 

//Functions
public:
	SettingManager() = delete;
	SettingManager(const std::string& setFile);


	bool ExpandedIn(void) const { return expandIn; }
	bool ExpandedOut(void) const { return expandOut; }

	float GetAlpha(void) const { return alpha; }
	unsigned GetBufferSize(void) const { return bufferSize; }
	Clipping GetClipping(void) const { return netC; }
	float GetClipThreshold(void) const { return clipThreshold; }
	float GetEta(void) const { return eta; }
	Initialization GetInit(void) const { return netI; }
	float GetLeak(void) const { return leak; }
	Loss GetNetLoss(void) const { return netL; }
	Loss GetNetError(void) const { return netE; }
	bool GetRandTrainData(void) const { return randomizedTraining; }
	float GetReservePercentage(void) const { return reservePercentage; }
	bool GetSimpleRecurrency(void) const { return simpleRecurrency; }
	bool GetSmushedIn(void) const { return smushedInputs; }
	bool GetAutoSmush(void) const { return autoSmush; }
	float GetSmushVal(void) const { return smushBy; }
	bool GetMainDisplay(void) const { return mainDisplay; }
	bool GetHotToNum(void) const { return hotToNum; }

private:
	void InputSettings(const std::string& dataString);
	void ReadLineAndClean(std::ifstream& dataStream, std::string& dataString);
	bool Stob(const std::string& dataString) const;

};