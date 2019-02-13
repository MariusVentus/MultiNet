#include "Topology.h"

Topology::Topology(const unsigned inLayerCount)
{
	for (unsigned layerNum = 0; layerNum < inLayerCount; ++layerNum) {
		layerTop.push_back(nTypes());
	}
}

void Topology::CreateMixedLayer(unsigned layerNum, unsigned nCount)
{
	std::vector<unsigned> tmpTypeLayer;
	unsigned tmpTotal = 0;
	unsigned i = 0;
	if (nCount != 0) {
		tmpTypeLayer.push_back(1);
	}
	while (i < nCount - 1) {
		for (unsigned nTypeTmp = 2; nTypeTmp <= nTypeIndexLength && tmpTotal < (nCount - 1); ++nTypeTmp) {
			tmpTypeLayer.push_back(nTypeTmp);
			++i;
			++tmpTotal;
		}
	}
	FillLayerTypes(layerNum, tmpTypeLayer);
}

void Topology::CreateMonoLayer(unsigned layerNum, unsigned nCount, unsigned inType)
{
	std::vector<unsigned> tmpTypeLayer;
	for (unsigned i = 0; i < nCount; ++i) {
		tmpTypeLayer.push_back(inType);
	}
	FillLayerTypes(layerNum, tmpTypeLayer);
}

void Topology::FillLayerTypes(unsigned layerNum, const std::vector<unsigned>& inTypes)
{
	for (unsigned typeNum = 0; typeNum < inTypes.size(); ++typeNum) {
		layerTop[layerNum].push_back(inTypes[typeNum]);
	}
}
