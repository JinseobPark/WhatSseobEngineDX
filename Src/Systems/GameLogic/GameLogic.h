#pragma once
#include "Systems/System.h"

class Object;
class GameLogic : public System
{
public:
	GameLogic();
	~GameLogic();

	void Initialize() override;
	void Update(float dt) override;


private:
	// temporary only one camera
	Object* camera = nullptr;
};
extern GameLogic* mGAMELOGIC;
