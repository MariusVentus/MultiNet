#include <assert.h>
#include <iostream>
#include <sstream>
#include "NeuralNet.h"
#include <future>

NeuralNet::NeuralNet(const Topology& netTop, const SettingManager& netSet)
	:
	m_netSet(netSet),
	m_rng(m_rd())
{
	unsigned numLayers = netTop.GetLayerCount();
	for (unsigned layerNum = 0; layerNum < numLayers; layerNum++) {
		m_layers.push_back(Layer());
		std::cout << "Layer" << layerNum << ": ";
		//Always add +1 to the Num Inputs, because the Bias is added outside the Topology.
		unsigned numInputs = layerNum == 0 ? 0 : netTop.GetNCount(layerNum - 1) + 1;
		unsigned numOutputs = layerNum == netTop.GetLayerCount() - 1 ? 0 : netTop.GetNCount(layerNum + 1);

		for (unsigned neuronNum = 0; neuronNum < netTop.GetNCount(layerNum); neuronNum++) {
			m_layers.back().push_back(Neuron(m_netSet, numInputs, numOutputs, neuronNum, netTop.GetNType(layerNum, neuronNum)));
		}
		//Add Bias - Fun fact, the NCount of your own layer does not include the bias, as it is added last!
		m_layers.back().push_back(Neuron(m_netSet, 0, numOutputs, netTop.GetNCount(layerNum), 0));
		//Set Layer Bias to one. 
		m_layers.back().back().SetOutputVal(1.0);
		std::cout << "\n";

	}
}

void NeuralNet::OverwriteNeuralNet(const std::string& fileData)
{
	m_layers.clear();
	std::cout << "Generating Neural Network from Save. Caution: Will use current Settings.";
	std::stringstream inNet(fileData);
	std::string token = "";

	unsigned lIndex = 0;
	unsigned nIndex = 0;
	std::vector<float> wVec;
	do {
		token.clear();
		std::getline(inNet, token, ',');
		if (token != "") {
			if (token.find("L") != std::string::npos) {
				m_layers.push_back(Layer());
				std::cout << "\nLayer" << lIndex << ": "; //Really need to get this console code out of the actual network.
				nIndex = 0;
				lIndex++;
			}
			else if (token.find("N") != std::string::npos) {
				//Add Vec, Clear, then create new Neuron
				if (nIndex != 0) {
					m_layers.back().back().OverwriteWeights(wVec);
					wVec.clear();
				}
				//Begin New Neuron spot. 
				std::string type = token.substr(token.find("N") + 1);
				m_layers.back().push_back(Neuron(m_netSet, 0, 0, nIndex, std::stoul(type)));
				if (std::stoul(type) == 0) { m_layers.back().back().SetOutputVal(1.0f); }
				nIndex++;
			}
			else {
				wVec.push_back(std::stof(token));
			}
		}
	} while (!inNet.eof());
	std::cout << "\n\n";
}


void NeuralNet::FeedForward(const std::vector<float>& inputVals)
{
	assert(inputVals.size() == m_layers[0].size() - 1);
	// Add -1 since t does not need to check Bias Neuron
	for (unsigned i = 0; i < inputVals.size(); i++) {
		m_layers[0][i].SetOutputVal(inputVals[i]);
	}

	//Dropout - Only on the hidden layers for now. Revive after Backprop. 
	if (m_netSet.IsDropoutActive() && IsTraining()) {
		std::uniform_int_distribution<unsigned> dist(0, 100);
		for (unsigned layerNum = 1; layerNum < m_layers.size() - 1; ++layerNum) {
			if (m_layers[layerNum][0].GetMyType() != 99) {
				//Leave -1, as Bias always alive. 
				for (unsigned n = 0; n < m_layers[layerNum].size() - 1; ++n) {
					if (static_cast<float>(dist(m_rng)) < m_netSet.GetDropout()) {
						m_layers[layerNum][n].KillNeuron();
					}
				}
			}
		}
	}

	//Start from one, as the Input Layer does not need to be Fed.
	for (unsigned layerNum = 1; layerNum < m_layers.size(); ++layerNum) {
		Layer& prevLayer = m_layers[layerNum - 1];
		//Normal or SoftMax Layer
		if (m_layers[layerNum][0].GetMyType() != 99) {
			unsigned ThreadCount = 20; // Temp until Setting Added. 
			unsigned lSize = (m_layers[layerNum].size() - 1);
			ThreadCount = std::min(ThreadCount, lSize);
			unsigned modVal = (lSize % ThreadCount);

			unsigned iMultiplier = (lSize - modVal) / ThreadCount;
			std::vector<std::future<void>> fHolding(ThreadCount);
			
			for (unsigned tc = 0; tc < ThreadCount; tc++) {
				//Async fun through Neurons
				fHolding[tc] = std::async(std::launch::async, [=, &prevLayer]() {
					for (unsigned n = (tc * iMultiplier); n < ((tc + 1) * iMultiplier); ++n) {
							m_layers[layerNum][n].FeedForward(prevLayer);
						}
					}
				);
			}
			if (modVal != 0) {
				for (unsigned n = lSize - modVal; n < lSize; ++n) {
					m_layers[layerNum][n].FeedForward(prevLayer);
				}
			}
			for (unsigned tc = 0; tc < ThreadCount; tc++) {
				fHolding[tc].wait();
			}
		}
		else {
			Layer& currentLayer = m_layers[layerNum];
			for (unsigned n = 0; n < m_layers[layerNum].size() - 1; ++n) {
				m_layers[layerNum][n].FeedForwardSM(prevLayer, currentLayer);
			}
		}
	}
}

void NeuralNet::BackProp(const std::vector<float>& targetVals)
{
	Layer& outputLayer = m_layers.back();
//Error
	RefreshError(targetVals);

//Output Gradients
	float clipNormal = 0.0f;
	for (unsigned n = 0; n < outputLayer.size() - 1; ++n) {
		outputLayer[n].CalcOutputGradients(targetVals[n]);
		//Output Norm Clipping
		if (m_netSet.GetClipping() == SettingManager::Clipping::NormClip) {
			clipNormal += (outputLayer[n].GetGradients()*outputLayer[n].GetGradients());
		}
	}
	//Output Norm Clipping Continued
	if (m_netSet.GetClipping() == SettingManager::Clipping::NormClip) {
		clipNormal = sqrt(clipNormal);
		for (unsigned n = 0; n < outputLayer.size() - 1; ++n) {
			outputLayer[n].NormClipping(clipNormal);
		}
	}

//Hidden Gradients
	for (unsigned layerNum = unsigned(m_layers.size()) - 2; layerNum > 0; layerNum--) {
		Layer& hiddenLayer = m_layers[layerNum];
		Layer& nextLayer = m_layers[layerNum + 1];
		clipNormal = 0.0f;

		//Last Neuron in a Layer is always Bias. Slight optimization with -1. 
		for (unsigned n = 0; n < hiddenLayer.size() - 1; ++n) {
			hiddenLayer[n].CalcHiddenGradients(nextLayer);
			if (m_netSet.GetClipping() == SettingManager::Clipping::NormClip) {
				//Hidden Norm Clipping
				if (hiddenLayer[n].IsAlive()) {
					clipNormal += (hiddenLayer[n].GetGradients()*hiddenLayer[n].GetGradients());
				}
			}
		}
		//Hidden Norm Clipping Continued
		if (m_netSet.GetClipping() == SettingManager::Clipping::NormClip) {
			clipNormal = sqrt(clipNormal);
			for (unsigned n = 0; n < hiddenLayer.size(); ++n) {
				hiddenLayer[n].NormClipping(clipNormal);
			}
		}
	}
//Update Weights
	
	for (unsigned layerNum = unsigned(m_layers.size()) - 1; layerNum > 0; layerNum--) {
		Layer& layer = m_layers[layerNum];
		Layer& prevLayer = m_layers[layerNum - 1];
		//Async Prep Area
		unsigned ThreadCount = 20; //Temp until Setting Added
		unsigned lSize = (layer.size() - 1);
		ThreadCount = std::min(ThreadCount, lSize);
		unsigned modVal = (lSize % ThreadCount);

		unsigned iMultiplier = (lSize - modVal) / ThreadCount;
		std::vector<std::future<void>> fHolding(ThreadCount);
		
		for (unsigned tc = 0; tc < ThreadCount; tc++) {
			//Async fun through Neurons
			fHolding[tc] = std::async(std::launch::async, [=, &layer, &prevLayer]() {
					for (unsigned n = (tc * iMultiplier); n < ((tc + 1) * iMultiplier); ++n) {
						layer[n].UpdateInputWeights(prevLayer);
					}
				}
			);
		}
		if (modVal != 0) {
			for (unsigned n = lSize - modVal; n < lSize; ++n) {
				layer[n].UpdateInputWeights(prevLayer);
			}
		}
		for (unsigned tc = 0; tc < ThreadCount; tc++) {
			fHolding[tc].wait();
		}

	}

	//Dropout - Revive Neurons
	if (m_netSet.IsDropoutActive()) {
		for (unsigned layerNum = 1; layerNum < m_layers.size() - 1; ++layerNum) {
			if (m_layers[layerNum][0].GetMyType() != 99) {
				for (unsigned n = 0; n < m_layers[layerNum].size(); ++n) {
					m_layers[layerNum][n].ReviveNeuron();
				}
			}
		}
	}

}

void NeuralNet::GetResults(std::vector<float>& resultVals) const
{
	resultVals.clear();
	// Don't forget to -1, because no Bias on the output. 
	for (unsigned n = 0; n < m_layers.back().size() - 1; n++) {
		resultVals.push_back(m_layers.back()[n].GetOutputVal());
	}
}

void NeuralNet::ClearNetMemory(void)
{
	for (unsigned i = 0; i < m_layers.size(); i++) {
		for (unsigned j = 0; j < m_layers[i].size(); j++) {
			m_layers[i][j].ClearCellMemory();
		}
	}
}

void NeuralNet::FullRevive(void)
{
	//Dropout - Revive Neurons
	for (unsigned layerNum = 0; layerNum < m_layers.size(); ++layerNum) {
		for (unsigned n = 0; n < m_layers[layerNum].size(); ++n) {
			m_layers[layerNum][n].ReviveNeuron();
		}
	}
}

void NeuralNet::SetTraining(const bool & set)
{
	m_isTraining = set;
	for (unsigned layerNum = 0; layerNum < m_layers.size(); ++layerNum) {
		for (unsigned n = 0; n < m_layers[layerNum].size(); ++n) {
			m_layers[layerNum][n].SetTraining(set);
		}
	}
}

void NeuralNet::RefreshError(const std::vector<float>& targetVals)
{
	Layer& outputLayer = m_layers.back();
	m_error = 0.0f;

	//Error
	if (m_netSet.GetNetError() == SettingManager::Loss::LogLoss) {
		//Log Loss
		if (outputLayer.size() > 2 && outputLayer[0].GetMyType() == 99) {
			//Greater than Single Output (+Bias), and with Softmax
			for (unsigned n = 0; n < outputLayer.size() - 1; ++n) {
				float delta = (targetVals[n] * log(outputLayer[n].GetOutputVal()));
				m_error += delta;
			}
			m_error = -m_error;
		}
		else {
			for (unsigned n = 0; n < outputLayer.size() - 1; ++n) {
				float delta = (targetVals[n] * log(outputLayer[n].GetOutputVal())) + ((1 - targetVals[n])*log(1 - outputLayer[n].GetOutputVal()));
				m_error += delta;
			}
			m_error = -m_error;
			m_error /= (outputLayer.size() - 1);
		}
	}
	else {
		//Mean Squared Error
		for (unsigned n = 0; n < outputLayer.size() - 1; ++n) {
			float delta = targetVals[n] - outputLayer[n].GetOutputVal();
			m_error += delta * delta;
		}
		m_error /= outputLayer.size() - 1;
		m_error = sqrt(m_error);
	}
}
