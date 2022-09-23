#pragma once
#include "d3dUtil.h"
#include "GameTimer.h"

class ImguiClass
{
public:
	void ImguiInitialize();
	void ImguiLoadInfo();
	void ImguiUpdate(const GameTimer& gt);
	void ImguiShutdown();

private:

};