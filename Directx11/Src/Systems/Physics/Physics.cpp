#include "stdafx.h"
#include "Physics.h"

Physics* mPHYSICS = nullptr;

Physics::Physics()
{
	assert(mPHYSICS == nullptr && "No more than one instance of Application system!");
	mPHYSICS = this;
}

Physics::~Physics()
{
}

void Physics::Initialize()
{
}

void Physics::Update(float dt)
{
}
