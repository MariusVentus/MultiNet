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
	for (unsigned epochs = 0; epochs < epochMax; ++epochs) {
		TimeKeeper EpochTime;
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
			}
			m_Input.ReloadBuffer();
			m_Output.ReloadBuffer();
		}
		std::cout << "Epoch Time: " << EpochTime.Mark() << "s \n";
		//std::cin.get(); //Test Pause - Single Epoch
		m_Input.ResetEoF();
		m_Input.ResetLoadedCount();
		m_Output.ResetLoadedCount();
	}
	std::cout << "\nTraining Time: " << TrainTime.Mark() << "s \n";
	std::cout << "\n";
}

void Cortex::Test(void)
{
	if (m_Input.GetMaxInputs() - m_TrainingDataSize) {
		TimeKeeper TestTime;
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
