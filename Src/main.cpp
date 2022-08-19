////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
// 메인 진입점
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Engine/WhatSseobEngine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Engine* WhatSseobEngine;
	bool result;

	WhatSseobEngine = new Engine;
	if (!WhatSseobEngine) return 0;

	// Initialize and run the system object.
	result = WhatSseobEngine->Initialize();
	if (result)
	{
		WhatSseobEngine->Run();
	}

	// Shutdown and release the system object.
	WhatSseobEngine->Shutdown();
	delete WhatSseobEngine;
	WhatSseobEngine = 0;

	return 0;
}