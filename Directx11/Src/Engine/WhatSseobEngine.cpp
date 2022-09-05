#include "stdafx.h"
#include "Systems/Application/Application.h"
#include "Systems/Input/inputClass.h"
#include "Systems/Sound/SoundClass.h"
#include "Systems/Timer/TimerClass.h"
#include "Systems/GameLogic/GameLogic.h"
#include "Systems/Physics/Physics.h"
#include "Systems/Graphics/Graphics.h"
#include "Object/ObjectFactory.h"
#include "Object/ObjectClass.h"
#include "Utils/Imgui/imgui.h"
#include "Utils/Imgui/imgui_impl_dx11.h"
#include "Utils/Imgui/imgui_impl_win32.h"
#include "Component/Transform.h"
#include "WhatSseobEngine.h"

Engine* ENGINE = nullptr;

Engine::Engine()
{
	assert(ENGINE == nullptr && "More than onc instance of the Engine cannot be created!");
}

Engine::Engine(const Engine& other)
{
}


Engine::~Engine()
{
	DestroyAllSystems();
}

bool Engine::Initialize()
{

	bool result;
	int screenWidth = 800, screenHeight = 600;

	//Add Systems
	AddSystem(new Application());
	AddSystem(new Graphics());
	AddSystem(new ObjectFactory());
	AddSystem(new InputClass());
	AddSystem(new TimerClass());
	AddSystem(new SoundClass());
	AddSystem(new Physics());
	AddSystem(new GameLogic());


	for (auto sys : gameSystems)
		sys->Initialize();

	mGRAPHICS->InitializeLightInfo();

	return true;
}

void Engine::DestroyAllSystems()
{
	for (auto sys = gameSystems.rbegin(); sys < gameSystems.rend(); ++sys)
		delete* sys;
}

void Engine::Shutdown()
{
	return;
}

void Engine::AddSystem(System* system)
{
	gameSystems.push_back(system);
}

void Engine::Run()
{
	MSG msg;
	bool done, result;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Update();
			if (!result)
			{
				done = true;
			}
		}

		// if input ESC, exit
		if (mINPUT->IsEscapePressed() == true)
		{
			break;
		}
	}

	return;

}

bool Engine::Update()
{
	XMFLOAT3 position(0.0f, 0.0f, 0.0f);

	bool result;
	float dt;
	// Update the system stats.
	dt = mTIMER->GetTime();

	for (auto sys : gameSystems)
	{
		sys->Update(dt);
	}


	return true;
}
