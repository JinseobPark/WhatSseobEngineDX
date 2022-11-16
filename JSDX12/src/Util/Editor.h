#pragma once

#include "../Object/RenderItem.h"
#include "../Datas.h"

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
	ROTATION_Z,
	NONE
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

bool material_getter(void* data, int index, const char** output);
bool material_getter2(void* data, int index, const char** output);
bool geo_getter(void* data, int index, const char** output);
bool layer_getter(void* data, int index, const char** output);

