#include "Cortex.h"
#include <assert.h>
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
	std::cin.get();

}

void Cortex::Train(void)
{
	for (unsigned epochs = 0; epochs < 1; ++epochs) {
		while (!m_Input.GetEoF()) {
			if (m_Settings.GetRandTrainData() == true) {
				m_rng.GenNShuffle(m_Input.GetBuffSize());
			}
			else {
				m_rng.GenSelectArray(m_Input.GetBuffSize());
			}
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
				std::cout << "\n";
				std::cout << "Input: ";
				for (unsigned j = 0; j < m_Input.GetRowSize(); j++) {
					std::cout << m_Input.GetRowX(m_rng.GetSelect(i))[j] << " ";
				}
				std::cout << "\n";

				std::cout << "Output: ";
				for (unsigned j = 0; j < m_Output.GetRowSize(); j++) {
					std::cout << resultVals[j] << " ";
				}
				std::cout << "\n";

				std::cout << "Target: ";
				for (unsigned j = 0; j < m_Output.GetRowSize(); j++) {
					std::cout << m_Output.GetRowX(m_rng.GetSelect(i))[j] << " ";
				}
				std::cout << "\n";
			}
			m_Input.ReloadBuffer();
			m_Output.ReloadBuffer();
		}
		m_Input.ResetEoF();
	}
}
