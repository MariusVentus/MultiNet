#include <assert.h>
#include <iostream>
#include "NeuralNet.h"

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
			for (unsigned n = 0; n < m_layers[layerNum].size() - 1; ++n) {
				m_layers[layerNum][n].FeedForward(prevLayer);
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
	float normL2 = 0.0f;
	for (unsigned n = 0; n < outputLayer.size() - 1; ++n) {
		outputLayer[n].CalcOutputGradients(targetVals[n]);
		//Output l2 Norm Clipping
		if (m_netSet.GetClipping() == SettingManager::Clipping::L2Clip) {
			normL2 += (outputLayer[n].GetGradients()*outputLayer[n].GetGradients());
		}
	}
	//Output l2 Norm Clipping Continued
	if (m_netSet.GetClipping() == SettingManager::Clipping::L2Clip) {
		normL2 = sqrt(normL2);
		for (unsigned n = 0; n < outputLayer.size() - 1; ++n) {
			outputLayer[n].NormClipping(normL2);
		}
	}

//Hidden Gradients
	for (unsigned layerNum = unsigned(m_layers.size()) - 2; layerNum > 0; layerNum--) {
		Layer& hiddenLayer = m_layers[layerNum];
		Layer& nextLayer = m_layers[layerNum + 1];
		normL2 = 0.0f;

		//Last Neuron in a Layer is always Bias. Slight optimization with -1. 
		for (unsigned n = 0; n < hiddenLayer.size() - 1; ++n) {
			hiddenLayer[n].CalcHiddenGradients(nextLayer);
			if (m_netSet.GetClipping() == SettingManager::Clipping::L2Clip) {
				//Hidden l2 Norm Clipping
				if (hiddenLayer[n].IsAlive()) {
					normL2 += (hiddenLayer[n].GetGradients()*hiddenLayer[n].GetGradients());
				}
			}
		}
		//Hidden l2 Norm Clipping Continued
		if (m_netSet.GetClipping() == SettingManager::Clipping::L2Clip) {
			normL2 = sqrt(normL2);
			for (unsigned n = 0; n < hiddenLayer.size(); ++n) {
				hiddenLayer[n].NormClipping(normL2);
			}
		}
	}
//Update Weights
	for (unsigned layerNum = unsigned(m_layers.size()) - 1; layerNum > 0; layerNum--) {
		Layer& layer = m_layers[layerNum];
		Layer& prevLayer = m_layers[layerNum - 1];

		for (unsigned n = 0; n < layer.size() - 1; ++n) {
			layer[n].UpdateInputWeights(prevLayer);
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
