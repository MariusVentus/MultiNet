#include "SaveHandler.h"
#include <fstream>

SaveHandler::SaveHandler(const NeuralNet& inNet)
{
	m_Network = "";
	for (unsigned i = 0; i < inNet.GetLayerCount(); i++) {
		m_Network.append("\nL");
		for (unsigned j = 0; j < inNet.GetNCount(i); j++) {
			m_Network.append(",N" + (std::to_string(inNet.GetNType(i, j))));
			
			std::vector<float> layerWeights = inNet.GetNWeightVec(i, j);
			for (unsigned k = 0; k < layerWeights.size(); k++) {
				m_Network.append("," + std::to_string(layerWeights[k]));
			}
		}
	}
	
	std::ofstream out("Saves\\Save.txt", std::ofstream::trunc);
	out << m_Network;
	out.close();

}