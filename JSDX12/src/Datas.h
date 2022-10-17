#pragma once
#include "stdafx.h"
#include "../Object/RenderItem.h"

using namespace DirectX;


struct SelectedObjectDatas
{
	bool isVisible = true;
};

struct ImguiData
{
	XMFLOAT3 pickposition = { 0, 0, 0 };
	bool isShowDebug = true;
	int offset = 0;

	bool isSelected = false;
	SelectedObjectDatas pickedData;

};

static ImguiData imguidata;