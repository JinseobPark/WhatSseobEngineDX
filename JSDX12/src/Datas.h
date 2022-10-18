#pragma once
#include "stdafx.h"
#include "../Object/RenderItem.h"
#include <string>
#include "Vector3.h"

using namespace DirectX;


struct SelectedObjectDatas
{
	std::string name;
	bool isVisible = true;
	float position[3] = { 0 };
	float scale[3] = { 1.0f };
	float rotation[3] = { 0 };
	float texscale[3] = { 0 };
};

struct ImguiData
{
	XMFLOAT3 pickposition = { 0, 0, 0 };
	bool isShowDebug = true;
	int offset = 0;

	bool isSelected = false;
	SelectedObjectDatas pickedData;

};
