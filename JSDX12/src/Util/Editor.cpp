#include "stdafx.h"
#include "Editor.h"

EditClass::EditClass()
{
}

EditClass::~EditClass()
{
}

EditModes EditClass::GetCurrMode()
{
	return currMode;
}

void EditClass::ChangeMode(int input)
{
	switch (input)
	{
		case 1: 
		{
			currMode = EditModes::PICK;
			return;
		}
		case 2:
		{
			if ((int)currMode >= (int)EditModes::TRANSFORM_X && (int)currMode <= (int)EditModes::TRANSFORM_Z)
				currMode = static_cast<EditModes>(((int)currMode) % 3 + 1);
			else
				currMode = EditModes::TRANSFORM_X;
			return;
		}
		case 3:
		{
			if ((int)currMode >= (int)EditModes::SCALE_X && (int)currMode <= (int)EditModes::SCALE_Z)
				currMode = static_cast<EditModes>(((int)currMode) % 3 + 4);
			else
				currMode = EditModes::SCALE_X;
			return;
		}
		case 4:
		{
			if ((int)currMode >= (int)EditModes::ROTATION_X && (int)currMode <= (int)EditModes::ROTATION_Z)
				currMode = static_cast<EditModes>(((int)currMode) % 3 + 7);
			else
				currMode = EditModes::ROTATION_X;
			return;
		}
		default : currMode = EditModes::PICK;

	}
}

void EditClass::UpdateTransform(float dx, float dy, RenderItem* item, RenderItem* itemview)
{
	if (item != nullptr)
	{
		switch (currMode)
		{
			case EditModes::TRANSFORM_X:
			{
				item->wTrans.x += dx;
				item->wTrans.y += dy;
				itemview->wTrans.x += dx;
				itemview->wTrans.y += dy;
				break;
			}
			case EditModes::TRANSFORM_Y:
			{
				item->wTrans.z += dx;
				item->wTrans.y += dy;
				itemview->wTrans.z += dx;
				itemview->wTrans.y += dy;
				break;
			}
			case EditModes::TRANSFORM_Z:
			{
				item->wTrans.x += dx;
				item->wTrans.z += dy;
				itemview->wTrans.x += dx;
				itemview->wTrans.z += dy;
				break;
			}
			case EditModes::SCALE_X:
			{
				item->wScale.x += dx;
				if (item->wScale.x == 0.0f) item->wScale.x += 0.001f;
				itemview->wScale.x = item->wScale.x;
				break;
			}
			case EditModes::SCALE_Y:
			{
				item->wScale.y += dy;
				if (item->wScale.y == 0.0f) item->wScale.y += 0.001f;
				itemview->wScale.y = item->wScale.y;
				break;
			}
			case EditModes::SCALE_Z:
			{
				item->wScale.z += dx;
				if (item->wScale.z == 0.0f) item->wScale.z += 0.001f;
				itemview->wScale.z = item->wScale.z;
				break;
			}
			case EditModes::ROTATION_X:
			{
				item->wRot.x += dx * 0.25f;
				itemview->wRot.x += dx * 0.25f;
				break;
			}
			case EditModes::ROTATION_Y:
			{
				item->wRot.y += dy * 0.25f;
				itemview->wRot.y += dy * 0.25f;
				break;
			}
			case EditModes::ROTATION_Z:
			{
				item->wRot.z += dx * 0.25f;
				itemview->wRot.z += dx * 0.25f;
				break;
			}
			default :
			{
				break;
			}

		}
		XMStoreFloat4x4(&item->World, MakeMatrixWorld(item->wRot, item->wScale, item->wTrans));
		item->NumFramesDirty = gNumFrameResources;
		XMStoreFloat4x4(&itemview->World, MakeMatrixWorld(itemview->wRot, itemview->wScale, itemview->wTrans));
		itemview->NumFramesDirty = gNumFrameResources;
	}
	
}
