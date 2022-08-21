#include "SaveHandler.h"
#include <fstream>

//Saving
SaveHandler::SaveHandler(const NeuralNet& inNet)
{
	m_Network = "";
	for (unsigned i = 0; i < inNet.GetLayerCount(); i++) {
		m_Network.append(",L");
		for (unsigned j = 0; j < inNet.GetNCount(i); j++) {
			m_Network.append(",N" + (std::to_string(inNet.GetNType(i, j))));
			
			std::vector<float> layerWeights = inNet.GetNWeightVec(i, j);
			for (unsigned k = 0; k < layerWeights.size(); k++) {
				m_Network.append("," + std::to_string(layerWeights[k]));
			}
		}
	}
	m_Network.erase(0, 1);
	std::ofstream out("Saves\\Save.txt", std::ofstream::trunc);
	out << m_Network;
	out.close();

}

//Loading
SaveHandler::SaveHandler(const std::string& saveFile)
{
	std::ifstream in(saveFile);
	m_Network = "";
	if (in) {
		std::string token = "";
		do {
			token.clear();
			std::getline(in, token);
			//Any Cleanup of Token can go here if needed
			m_Network.append(token);

		} while (!in.eof());
	}

}
