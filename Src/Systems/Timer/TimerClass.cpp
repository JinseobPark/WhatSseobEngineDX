#include "stdafx.h"
#include "TimerClass.h"

TimerClass* mTIMER = nullptr;
TimerClass::TimerClass()
{
	assert(mTIMER == nullptr && "No more than one instance of Application system!");
	mTIMER = this;
}


TimerClass::TimerClass(const TimerClass& other)
{
}


TimerClass::~TimerClass()
{
}


void TimerClass::Initialize()
{
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if (m_frequency == 0)
	{
		return ;
	}

	// Find out how many times the frequency counter ticks every millisecond.
	m_ticksPerMs = (float)(m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return ;
}


void TimerClass::Update(float)
{
	INT64 currentTime = 0;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	float timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;

	m_startTime = currentTime;
}


float TimerClass::GetTime()
{
	return m_frameTime;
}