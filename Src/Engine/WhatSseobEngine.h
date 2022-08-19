#pragma once
#include <string>
#include <vector>

class InputClass;
class TimerClass;
class GraphicsClass;
class SoundClass;

class System;
class Engine
{
public:
	Engine();
	Engine(const Engine&);
	~Engine();

	bool Initialize();
	void Run();

	void DestroyAllSystems();
	void Shutdown();



private:
	bool Update();
	void AddSystem(System* system);

private:
	std::vector<System*> gameSystems;
};

extern Engine* ENGINE;