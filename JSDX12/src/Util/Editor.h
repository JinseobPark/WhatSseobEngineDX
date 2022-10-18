#pragma once
#include "../Object/RenderItem.h"

class RenderItem;

enum class EditModes
{
	PICK = 0,
	TRANSFORM_X,
	TRANSFORM_Y,
	TRANSFORM_Z,
	SCALE_X,
	SCALE_Y,
	SCALE_Z,
	ROTATION_X,
	ROTATION_Y,
	ROTATION_Z
};

class EditClass
{
public:
	EditClass();
	~EditClass();

	EditModes GetCurrMode();

	void ChangeMode(int input);

	void UpdateTransform(float dx, float dy, RenderItem* item, RenderItem* itemview);

private:
	EditModes currMode = EditModes::PICK;
};