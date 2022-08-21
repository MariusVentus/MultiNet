#include "Cortex.h"
#include "TimeKeeper.h"
#include "SaveHandler.h"
#include <assert.h>
#include <cmath>
#include <iostream>
#include <sstream>

Cortex::Cortex(const std::string& settingFile, const std::string& topologyFile, const std::string& inputFile, const std::string& outputFile)
	:
	m_Settings(settingFile),
	m_Input(m_Settings, inputFile),
	m_Output(m_Settings, outputFile),
	m_Top(topologyFile, m_Input.GetIRowSize(), m_Output.GetORowSize()),
	m_NN(m_Top, m_Settings)
{
	assert(m_Input.GetBuffSize() == m_Output.GetBuffSize());
	//Must set InputCount at the beginning, as it needs to use a loaded buffer.
	m_InputCount = m_Input.GetIRowSize();
	m_TrainingDataSize = static_cast<unsigned>(std::round(m_Input.GetMaxInputs()*((100.0f - m_Settings.GetReservePercentage()) / 100.0f)));
	std::cout << "\nTraining on " << m_TrainingDataSize << " sets, reserving " << m_Input.GetMaxInputs() - m_TrainingDataSize << " sets. \n\n";
}

void Cortex::ManualRun(const std::string& manIn)
{
	//Setup and Input
	std::vector<float> manualInputs;
	std::stringstream lineStream(manIn);
	std::string	cell;
	while (std::getline(lineStream, cell, ',')) {
		manualInputs.emplace_back(stof(cell));
	}
	if (m_Settings.ExpandedIn() && manualInputs.size() == 1) {
		std::vector<float> tempBuff;
		for (unsigned i = 0; i <= m_Input.GetMaxValArrX(0); i++) {
			if (manualInputs[0] != i) {
				tempBuff.emplace_back(0.0f);
			}
			else {
				tempBuff.emplace_back(1.0f);
			}
		}
		manualInputs = tempBuff;
	}

	//Run
	std::cout << "Running!\n";
	m_NN.SetTraining(false);
	if (m_Settings.IsDropoutActive()) {
		m_NN.FullRevive();
	}

	if (manualInputs.size() == m_InputCount) {
		std::vector<float> resultVals;
		//Feed
		if (m_Settings.GetSmushedIn() && !m_Settings.ExpandedIn()) {
			for (unsigned i = 0; i < manualInputs.size(); i++) {

				//Add if for smushing
				if (m_Settings.GetAutoSmush()) {
					manualInputs[i] = manualInputs[i] / m_Input.GetMaxValArrX(i);
				}
				else {
					manualInputs[i] = manualInputs[i] / m_Settings.GetSmushVal();
				}
			}
		}
		m_NN.FeedForward(manualInputs);

		//Results
		m_NN.GetResults(resultVals);

		//Display Inputs
		std::cout << "\nResults!\n";
		std::cout << "Input: ";
		for (unsigned j = 0; j < manualInputs.size(); j++) {
			std::cout << manualInputs[j] << " ";
		}
		std::cout << "\n";

		//Display Results and Outputs
		std::cout << "Output: ";
		if (!m_Settings.GetHotToNum()) {
			for (unsigned j = 0; j < resultVals.size(); j++) {
				std::cout << resultVals[j] << " ";
			}
		}
		else {
			std::cout << DataHandler::GetMaxArrLoc(resultVals);
		}
		std::cout << "\n\n";
	}
	else {
		std::cout << "\nManual input size did not match normal input size.\n\n";
	}
}

void Cortex::Train(const unsigned& epochMax)
{
	TimeKeeper TrainTime;
	float totalAccuracy = 0.0f;
	unsigned trainCount = 0;
	m_NN.SetTraining(true);

	for (unsigned epochs = 0; epochs < epochMax; ++epochs) {
		TimeKeeper EpochTime;
		float epochAccuracy = 0.0f;
		unsigned epochCount = 0;
		float errorTot = 0.0f;
		
		if (m_Settings.GetSimpleRecurrency() && !m_Settings.IsMemPerm()) {
			m_NN.ClearNetMemory();
		}

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
				if (!m_Settings.ExpandedIn()) {
					if (!m_Settings.GetSmushedIn()) {
						m_NN.FeedForward(m_Input.GetRowX(m_rng.GetSelect(i)));
					}
					else {
						m_NN.FeedForward(m_Input.GetSmushedRowX(m_rng.GetSelect(i)));
					}
				}
				else {
					m_NN.FeedForward(m_Input.GetExpandedRowX(m_rng.GetSelect(i)));
				}
				//Results
				m_NN.GetResults(resultVals);
				//Correct
				if (!m_Settings.ExpandedOut()) {
					m_NN.BackProp(m_Output.GetRowX(m_rng.GetSelect(i)));
				}
				else {
					m_NN.BackProp(m_Output.GetExpandedRowX(m_rng.GetSelect(i)));
				}
				//Display
				if (m_Settings.GetMainDisplay()) {
					DisplayTraining(i, resultVals);
				}
				//Error
				errorTot += m_NN.GetError();
				epochCount++;
				std::cout << "Error: " << m_NN.GetError() << " Avg Epoch Error: " << errorTot / static_cast<float>(epochCount) << "\n";
				//Accuracy
				std::string correct = "False";
				if (!m_Settings.ExpandedOut()) {
					if (CheckAccuracy(resultVals, m_Output.GetRowX(m_rng.GetSelect(i)))) {
						epochAccuracy++;
						totalAccuracy++;
						correct = "True";
					}
				}
				else {
					if (CheckAccuracy(resultVals, m_Output.GetExpandedRowX(m_rng.GetSelect(i)))) {
						epochAccuracy++;
						totalAccuracy++;
						correct = "True";
					}
				}
				std::cout << "Correct:" << correct << " Accuracy: " << epochAccuracy / static_cast<float>(epochCount) << "\n";
				trainCount++;
			}
			m_Input.ReloadBuffer();
			m_Output.ReloadBuffer();
		}
		std::cout << "\nEpoch Accuracy: " << epochAccuracy / static_cast<float>(epochCount) << "\n";
		DisplayTime("Epoch", EpochTime.Mark());

		//std::cin.get(); //Test Pause - Single Epoch
		m_Input.ResetEoF();
		m_Input.ResetLoadedCount();
		m_Output.ResetLoadedCount();
	}
	std::cout << "\nTraining Accuracy: " << totalAccuracy / static_cast<float>(trainCount) << "\n";
	DisplayTime("Training", TrainTime.Mark());

	std::cout << "\n";
}

void Cortex::Test(void)
{
	if (m_Input.GetMaxInputs() != m_TrainingDataSize) {
		TimeKeeper TestTime;

		m_NN.SetTraining(false);
		if (m_Settings.IsDropoutActive()) {
			m_NN.FullRevive();
		}

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
				if (!m_Settings.ExpandedIn()) {
					if (!m_Settings.GetSmushedIn()) {
						m_NN.FeedForward(m_Input.GetRowX(i));
					}
					else {
						m_NN.FeedForward(m_Input.GetSmushedRowX(i));
					}
				}
				else {
					m_NN.FeedForward(m_Input.GetExpandedRowX(i));
				}
				//Results
				m_NN.GetResults(resultVals);
				//Display
				if (m_Settings.GetMainDisplay()) {
					DisplayTesting(i, resultVals);
				}
				//Error
				if (!m_Settings.ExpandedOut()) {
					m_NN.RefreshError(m_Output.GetRowX(i));
				}
				else {
					m_NN.RefreshError(m_Output.GetExpandedRowX(i));
				}
				
				errorTot += m_NN.GetError();
				testCount++;
				std::cout << "Error: " << m_NN.GetError() << " Avg Epoch Error: " << errorTot / static_cast<float>(testCount) << "\n";
				//Accuracy
				std::string correct = "False";
				if (!m_Settings.ExpandedOut()) {
					if (CheckAccuracy(resultVals, m_Output.GetRowX(i))) {
						testAccuracy++;
						correct = "True";
					}
				}
				else {
					if (CheckAccuracy(resultVals, m_Output.GetExpandedRowX(i))) {
						testAccuracy++;
						correct = "True";
					}
				}
				std::cout << "Correct:" << correct << " Test Accuracy: " << testAccuracy / static_cast<float>(testCount) << "\n";
			}
			m_Input.ReloadTestBuffer();
			m_Output.ReloadTestBuffer();
		}
		DisplayTime("Test", TestTime.Mark());
		std::cout << "\n";
		m_Input.ResetEoF();
	}
	else {
		std::cout << "Nothing to Test on. \n";
	}
	m_Input.ResetDh();
	m_Output.ResetDh();
}

void Cortex::Save(void)
{
	SaveHandler Saver(m_NN);
}

void Cortex::Load(void)
{
	SaveHandler Loader("Saves\\Save.txt");
	m_NN.OverwriteNeuralNet(Loader.GetNetString());
}

void Cortex::DisplayTesting(unsigned buffRow, const std::vector<float>& inResultVals) const
{
	//Inputs
	std::cout << "\n";
	std::cout << "Input: ";
	for (unsigned j = 0; j < m_Input.GetIRowSize(); j++) {
		std::cout << m_Input.GetRowX(buffRow)[j] << " ";
	}
	std::cout << "\n";

	if (m_Settings.GetSmushedIn()) {
		std::cout << "Smushed Input: ";
		for (unsigned j = 0; j < m_Input.GetIRowSize(); j++) {
			std::cout << m_Input.GetSmushedRowX(buffRow)[j] << " ";
		}
		std::cout << "\n";
	}
	//Results and Outputs
	std::cout << "Output: ";
	if (m_Settings.GetHotToNum() && m_Output.GetORowSize() != 1) {
		std::cout << DataHandler::GetMaxArrLoc(inResultVals);
	}
	else {
		for (unsigned j = 0; j < m_Output.GetORowSize(); j++) {
			std::cout << inResultVals[j] << " ";
		}
	}
	std::cout << "\n";

	//Targets
	if (m_Settings.ExpandedOut()) {
		std::cout << "Target: ";
		if (!m_Settings.GetHotToNum()) {
			for (unsigned j = 0; j < m_Output.GetORowSize(); j++) {
				std::cout << m_Output.GetExpandedRowX(buffRow)[j] << " ";
			}
		}
		else {
			std::cout << DataHandler::GetMaxArrLoc(m_Output.GetExpandedRowX(buffRow));
		}
		std::cout << "\n";
	}
	else {
		std::cout << "Target: ";
		if (!m_Settings.GetHotToNum()) {
			for (unsigned j = 0; j < m_Output.GetORowSize(); j++) {
				std::cout << m_Output.GetRowX(buffRow)[j] << " ";
			}
		}
		else {
			std::cout << DataHandler::GetMaxArrLoc(m_Output.GetRowX(buffRow));
		}
		std::cout << "\n";
	}
}

void Cortex::DisplayTraining(unsigned buffRow, const std::vector<float>& inResultVals) const
{
	//Inputs
	std::cout << "\n";
	std::cout << "Input: ";
	for (unsigned j = 0; j < m_Input.GetIRowSize(); j++) {
		std::cout << m_Input.GetRowX(m_rng.GetSelect(buffRow))[j] << " ";
	}
	std::cout << "\n";

	if (m_Settings.GetSmushedIn()) {
		std::cout << "Smushed Input: ";
		for (unsigned j = 0; j < m_Input.GetIRowSize(); j++) {
			std::cout << m_Input.GetSmushedRowX(m_rng.GetSelect(buffRow))[j] << " ";
		}
		std::cout << "\n";
	}
	//Results and Outputs
	std::cout << "Output: ";
	if (!m_Settings.GetHotToNum()) {
		for (unsigned j = 0; j < m_Output.GetORowSize(); j++) {
			std::cout << inResultVals[j] << " ";
		}
	}
	else {
		std::cout << DataHandler::GetMaxArrVal(inResultVals);
	}
	std::cout << "\n";
	//Targets
	if (m_Settings.ExpandedOut()) {
		std::cout << "Target: ";
		if (!m_Settings.GetHotToNum()) {
			for (unsigned j = 0; j < m_Output.GetORowSize(); j++) {
				std::cout << m_Output.GetExpandedRowX(m_rng.GetSelect(buffRow))[j] << " ";
			}
		}
		else {
			std::cout << DataHandler::GetMaxArrLoc(m_Output.GetExpandedRowX(m_rng.GetSelect(buffRow)));
		}
		std::cout << "\n";
	}
	else {
		std::cout << "Target: ";
		if (!m_Settings.GetHotToNum()) {
			for (unsigned j = 0; j < m_Output.GetORowSize(); j++) {
				std::cout << m_Output.GetRowX(m_rng.GetSelect(buffRow))[j] << " ";
			}
		}
		else {
			std::cout << DataHandler::GetMaxArrVal(m_Output.GetRowX(m_rng.GetSelect(buffRow)));
		}
		std::cout << "\n";
	}
}

void Cortex::DisplayTime(const std::string& timeType, const float& inTime) const
{
	if (inTime <= 60.0f) {
		std::cout << timeType << " Time: " << inTime << " seconds \n";
	}
	else if (inTime <= 3600.0f) {
		std::cout << timeType << " Time: " << inTime/60.0f << " minutes \n";
	}
	else {
		std::cout << timeType << " Time: " << inTime/3600.0f << " hours \n";
	}
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
