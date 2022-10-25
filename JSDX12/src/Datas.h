#pragma once
#include "stdafx.h"
#include "../Object/RenderItem.h"
#include <string>
#include "Vector3.h"

using namespace DirectX;


class ImGuiMaterials {
public:
	std::shared_ptr<Material> mat;
	std::string name;
};

class ImGuiGeos
{
public:
	std::string name;
};

class ImGuiLayers
{
public:
	std::string name;
};


struct SelectedObjectDatas
{
	std::string name;
	bool isVisible = true;
	float position[3] = { 0 };
	float scale[3] = { 1.0f };
	float rotation[3] = { 0 };
	float texscale[3] = { 0 };
	std::string matName;
	std::string geoName;
	int layerNum = 0;
	bool isLayerChanged = false;
};


struct ImguiData
{
	XMFLOAT3 pickposition = { 0, 0, 0 };
	bool isShowDebug = true;
	bool materialWindow = true;
	int offset = 0;

	bool isSelected = false;
	SelectedObjectDatas pickedData;
};
