#include "Cortex.h"
#include "TimeKeeper.h"
#include <assert.h>
#include <cmath>
#include <iostream>

Cortex::Cortex(const std::string& settingFile, const std::string& topologyFile, const std::string& inputFile, const std::string& outputFile)
	:
	m_Settings(settingFile),
	m_Input(m_Settings, inputFile),
	m_Output(m_Settings, outputFile),
	m_Top(topologyFile, m_Input.GetRowSize(), m_Output.GetRowSize()),
	m_NN(m_Top, m_Settings)
{
	assert(m_Input.GetBuffSize() == m_Output.GetBuffSize());
	m_TrainingDataSize = static_cast<unsigned>(std::round(m_Input.GetMaxInputs()*((100.0f - m_Settings.GetReservePercentage()) / 100.0f)));
	std::cout << "\nTraining on " << m_TrainingDataSize << " sets, reserving " << m_Input.GetMaxInputs() - m_TrainingDataSize << " sets. \n\n";
}

void Cortex::Train(const unsigned& epochMax)
{
	TimeKeeper TrainTime;
	float totalAccuracy = 0.0f;
	unsigned trainCount = 0;
	for (unsigned epochs = 0; epochs < epochMax; ++epochs) {
		TimeKeeper EpochTime;
		float epochAccuracy = 0.0f;
		unsigned epochCount = 0;
		float errorTot = 0.0f;
		//Run Epoch
		while (!m_Input.GetEoF()) {

			if (m_Settings.GetRandTrainData() == true) {
				m_rng.GenNShuffle(m_Input.GetBuffSize());
			}
			else {
				m_rng.GenSelectArray(m_Input.GetBuffSize());
			}
			//Run Buffer
			for (unsigned i = 0; i < m_Input.GetBuffSize(); i++) {
				assert(m_Input.GetBuffSize() == m_Output.GetBuffSize());
				std::vector<float> resultVals;
				//Feed
				m_NN.FeedForward(m_Input.GetRowX(m_rng.GetSelect(i)));
				//Results
				m_NN.GetResults(resultVals);
				//Correct
				m_NN.BackProp(m_Output.GetRowX(m_rng.GetSelect(i)));
				//Display
				DisplayTraining(i, resultVals);
				//Error
				errorTot += m_NN.GetError();
				epochCount++;
				std::cout << "Error: " << m_NN.GetError() << " Avg Epoch Error: " << errorTot / static_cast<float>(epochCount) << "\n";
				//Accuracy
				if (CheckAccuracy(resultVals, m_Output.GetRowX(m_rng.GetSelect(i)))) {
					epochAccuracy++;
					totalAccuracy++;
				}
				std::cout << "Accuracy: " << epochAccuracy / static_cast<float>(epochCount) << "\n";
				trainCount++;
			}
			m_Input.ReloadBuffer();
			m_Output.ReloadBuffer();
		}
		std::cout << "\nEpoch Accuracy: " << epochAccuracy / static_cast<float>(epochCount) << "\n";
		std::cout << "Epoch Time: " << EpochTime.Mark() << "s \n";
		//std::cin.get(); //Test Pause - Single Epoch
		m_Input.ResetEoF();
		m_Input.ResetLoadedCount();
		m_Output.ResetLoadedCount();
		if (m_Settings.GetSimpleRecurrency()) {
			m_NN.ClearNetMemory();
		}
	}
	std::cout << "\nTraining Accuracy: " << totalAccuracy / static_cast<float>(trainCount) << "\n";
	std::cout << "Training Time: " << TrainTime.Mark() << "s \n";
	std::cout << "\n";
}

void Cortex::Test(void)
{
	if (m_Input.GetMaxInputs() - m_TrainingDataSize) {
		TimeKeeper TestTime;
		unsigned testCount = 0;
		float errorTot = 0.0f;
		float testAccuracy = 0.0f;
		//Run Test
		m_Input.PrepTest();
		m_Output.PrepTest();
		m_Input.LoadTestBuff();
		m_Output.LoadTestBuff();
		while (!m_Input.GetEoF()) {
			//Run Buffer
			for (unsigned i = 0; i < m_Input.GetBuffSize(); i++) {
				assert(m_Input.GetBuffSize() == m_Output.GetBuffSize());
				std::vector<float> resultVals;
				//Feed
				m_NN.FeedForward(m_Input.GetRowX(i));
				//Results
				m_NN.GetResults(resultVals);
				//Display
				DisplayTesting(i, resultVals);
				//Error
				errorTot += m_NN.GetError();
				testCount++;
				std::cout << "Error: " << m_NN.GetError() << " Avg Epoch Error: " << errorTot / static_cast<float>(testCount) << "\n";
				//Accuracy
				if (CheckAccuracy(resultVals, m_Output.GetRowX(i))) {
					testAccuracy++;
				}
				std::cout << "Test Accuracy: " << testAccuracy / static_cast<float>(testCount) << "\n";
			}
			m_Input.ReloadTestBuffer();
			m_Output.ReloadTestBuffer();
		}
		std::cout << "Test Time: " << TestTime.Mark() << "s \n";

		m_Input.ResetEoF();
	}
	else {
		std::cout << "Nothing to Test on. \n";
	}
	m_Input.ResetDh();
	m_Output.ResetDh();
}

void Cortex::DisplayTesting(unsigned buffRow, const std::vector<float>& inResultVals) const
{
	//Display
	std::cout << "\n";
	std::cout << "Input: ";
	for (unsigned j = 0; j < m_Input.GetRowSize(); j++) {
		std::cout << m_Input.GetRowX(buffRow)[j] << " ";
	}
	std::cout << "\n";

	std::cout << "Output: ";
	for (unsigned j = 0; j < m_Output.GetRowSize(); j++) {
		std::cout << inResultVals[j] << " ";
	}
	std::cout << "\n";

	std::cout << "Target: ";
	for (unsigned j = 0; j < m_Output.GetRowSize(); j++) {
		std::cout << m_Output.GetRowX(buffRow)[j] << " ";
	}
	std::cout << "\n";
}

void Cortex::DisplayTraining(unsigned buffRow, const std::vector<float>& inResultVals) const
{
	//Display
	std::cout << "\n";
	std::cout << "Input: ";
	for (unsigned j = 0; j < m_Input.GetRowSize(); j++) {
		std::cout << m_Input.GetRowX(m_rng.GetSelect(buffRow))[j] << " ";
	}
	std::cout << "\n";

	std::cout << "Output: ";
	for (unsigned j = 0; j < m_Output.GetRowSize(); j++) {
		std::cout << inResultVals[j] << " ";
	}
	std::cout << "\n";

	std::cout << "Target: ";
	for (unsigned j = 0; j < m_Output.GetRowSize(); j++) {
		std::cout << m_Output.GetRowX(m_rng.GetSelect(buffRow))[j] << " ";
	}
	std::cout << "\n";
}

bool Cortex::CheckAccuracy(const std::vector<float>& output, const std::vector<float>& target) const
{
	if (m_Settings.GetNetLoss() == SettingManager::Loss::MeanSquared) {
		if (target.size() != 1) {
			for (unsigned i = 0; i < target.size(); i++) {
				if (output[i] > (target[i] + .5f) || output[i] < (target[i] - .5f)) {
					return false;
				}
			}
			return true;
		}
		else {
			if (output[0] < (target[0] + .5f) && output[0] > (target[0] - .5f)) {
				return true;
			}
			else {
				return false;
			}
		}
	}
	else if (m_Settings.GetNetLoss() == SettingManager::Loss::LogLoss) {
		if (target.size() != 1) {
			return DataHandler::GetMaxArrLoc(target) == DataHandler::GetMaxArrLoc(output);
		}
		else {
			std::cout << "\nCross Entropy with one output not supported. Accuracy will assume Mean Squared.\n";
			if (output[0] < (target[0] + .5f) && output[0] > (target[0] - .5f)) {
				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		return false;
	}
}
