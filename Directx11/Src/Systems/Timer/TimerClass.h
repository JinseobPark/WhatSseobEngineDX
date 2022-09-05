#pragma once
#include "Systems/System.h"

class TimerClass : public System
{
public:
	TimerClass();
	TimerClass(const TimerClass&);
	~TimerClass();

	void Initialize() override;
	void Update(float ) override;

	float GetTime();

private:
	INT64 m_frequency = 0;
	float m_ticksPerMs = 0;
	INT64 m_startTime = 0;
	float m_frameTime = 0;
};
extern TimerClass* mTIMER;