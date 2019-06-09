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
	unsigned m_BufferSize;
	bool m_RandomizedTraining;
	float m_ReservePercentage;
	bool m_SmushedInputs;
	bool m_AutoSmush;
	float m_SmushBy;
	bool m_ExpandIn;
	bool m_ExpandOut;

	//Display
	bool m_MainDisplay;
	bool m_HotToNum;

	//Dropout
	bool m_IsDropout;
	float m_DropoutPercentage;

	//Gradient Clipping
	Clipping m_NetC = Clipping::None;
	float m_ClipThreshold;

	//Initialization
	Initialization m_NetI;

	//Learning
	float m_Eta;
	float m_Alpha;
	float m_Leak; //For the Relu.

	//Loss (for Optimization) and Error (for reporting).
	Loss m_NetL;
	Loss m_NetE;

	//Simple Recurrency
	bool m_SimpleRecurrency; 
	bool m_RestrictLinear;
	bool m_PermMem;

//Functions
public:
	SettingManager() = delete;
	SettingManager(const std::string& setFile);


	bool ExpandedIn(void) const { return m_ExpandIn; }
	bool ExpandedOut(void) const { return m_ExpandOut; }

	float GetAlpha(void) const { return m_Alpha; }
	unsigned GetBufferSize(void) const { return m_BufferSize; }
	Clipping GetClipping(void) const { return m_NetC; }
	float GetClipThreshold(void) const { return m_ClipThreshold; }
	float GetEta(void) const { return m_Eta; }
	Initialization GetInit(void) const { return m_NetI; }
	float GetLeak(void) const { return m_Leak; }
	Loss GetNetLoss(void) const { return m_NetL; }
	Loss GetNetError(void) const { return m_NetE; }
	bool GetRandTrainData(void) const { return m_RandomizedTraining; }
	float GetReservePercentage(void) const { return m_ReservePercentage; }
	bool GetSimpleRecurrency(void) const { return m_SimpleRecurrency; }
	bool GetSmushedIn(void) const { return m_SmushedInputs; }
	bool GetAutoSmush(void) const { return m_AutoSmush; }
	float GetSmushVal(void) const { return m_SmushBy; }
	bool GetMainDisplay(void) const { return m_MainDisplay; }
	bool GetHotToNum(void) const { return m_HotToNum; }
	bool IsLinearRestricted(void) const { return m_RestrictLinear; }
	bool IsDropoutActive(void) const { return m_IsDropout; }
	float GetDropout(void) const { return m_DropoutPercentage; }
	bool IsMemPerm(void) const { return m_PermMem; }

private:
	void InputSettings(const std::string& dataString);
	void ReadLineAndClean(std::ifstream& dataStream, std::string& dataString);
	bool Stob(const std::string& dataString) const;

};