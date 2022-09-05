#pragma once
#include "Systems/System.h"

class Physics : public System
{
public:
	Physics();
	~Physics();

	void Initialize() override;
	void Update(float dt) override;


private:
};
extern Physics* mPHYSICS;