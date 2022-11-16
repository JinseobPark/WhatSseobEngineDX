#include "stdafx.h"
#include "RenderItems.h"

RenderItemClass::RenderItemClass()
{
	mJson = new JsonClass();
	mJson->JsonReadTest();
	mJson->JsonTest();
}

RenderItemClass::~RenderItemClass()
{
	delete mJson;
}


void RenderItemClass::BuildRenderItemsFromJson(std::unique_ptr<MaterialClass>* materials, std::unique_ptr<GeoMetryClass>* geometries)
{
	mJson->LoadFromJson(mAllRitems, mRitemLayer, materials, geometries, &objCBIndex);
	mPickedRitemview = mRitemLayer[(int)RenderLayer::Highlight][0];
}

void RenderItemClass::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, int layer, FrameResource* curFrames)
{
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	auto objectCB = curFrames->ObjectCB->Resource();
	auto ritems = mRitemLayer[layer];
	// For each render item...
	for (size_t i = 0; i < ritems.size(); ++i)
	{
		auto ri = ritems[i];

		if (ri->Visible == false)
			continue;

		cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;

		cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

std::vector<std::shared_ptr<RenderItem>> RenderItemClass::GetAllRiems()
{
	return mAllRitems;
}

void RenderItemClass::Pick(int sx, int sy, Camera camera, int widht, int height, ImguiData* data)
{
	XMFLOAT4X4 P = camera.GetProj4x4f();

	// Compute picking ray in view space.
	float vx = (+2.0f * sx / widht - 1.0f) / P(0, 0);
	float vy = (-2.0f * sy / height + 1.0f) / P(1, 1);

	// Ray definition in view space.
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	XMMATRIX V = camera.GetView();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

	rayOrigin = XMVector3TransformCoord(rayOrigin, invView);
	rayDir = XMVector3TransformNormal(rayDir, invView);
	// Assume nothing is picked to start, so the picked render-item is invisible.
	mPickedRitemview->Visible = false;
	data->isSelected = false;
	mPickedRitem = nullptr;

	float tmax = 65536.0f;
	// of objects that can be selected.   
	std::vector<RenderItem*> canpicked;
	canpicked.reserve(mRitemLayer[(int)RenderLayer::Opaque].size() + mRitemLayer[(int)RenderLayer::Transparent].size() + mRitemLayer[(int)RenderLayer::OpaqueDynamicReflectors].size() + mRitemLayer[(int)RenderLayer::Toon].size());
	canpicked.insert(canpicked.begin(), mRitemLayer[(int)RenderLayer::Opaque].begin(), mRitemLayer[(int)RenderLayer::Opaque].end());
	canpicked.insert(canpicked.end(), mRitemLayer[(int)RenderLayer::Transparent].begin(), mRitemLayer[(int)RenderLayer::Transparent].end());
	canpicked.insert(canpicked.end(), mRitemLayer[(int)RenderLayer::OpaqueDynamicReflectors].begin(), mRitemLayer[(int)RenderLayer::OpaqueDynamicReflectors].end());
	canpicked.insert(canpicked.end(), mRitemLayer[(int)RenderLayer::Toon].begin(), mRitemLayer[(int)RenderLayer::Toon].end());
	for (auto pickeditem : canpicked)
	{
		XMMATRIX W = XMLoadFloat4x4(&pickeditem->World);
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

		XMVECTOR newrayOrigin = XMVector3TransformCoord(rayOrigin, invWorld);
		XMVECTOR newrayDir = XMVector3TransformNormal(rayDir, invWorld);

		// Make the ray direction unit length for the intersection tests.
		newrayDir = XMVector3Normalize(newrayDir);

		float distance = 0.0f;
		if (pickeditem->Bounds.Intersects(newrayOrigin, newrayDir, distance))
		{
			if (distance < tmax)
			{
				tmax = distance;

				mPickedRitemview->Visible = true;

				// Picked render item needs same world matrix as object picked.
				mPickedRitemview->Geo = pickeditem->Geo;
				mPickedRitemview->World = pickeditem->World;
				mPickedRitemview->TexTransform = pickeditem->TexTransform;
				mPickedRitemview->NumFramesDirty = gNumFrameResources;

				// Offset to the picked triangle in the mesh index buffer.
				mPickedRitemview->IndexCount = pickeditem->IndexCount;
				mPickedRitemview->BaseVertexLocation = pickeditem->BaseVertexLocation;
				mPickedRitemview->StartIndexLocation = pickeditem->StartIndexLocation;

				mPickedRitem = pickeditem;
				data->isSelected = true;
			}

		}
	}

}

void RenderItemClass::GetPickedItemOnGui(ImguiData* data)
{
	if (mPickedRitem != nullptr)
	{
		data->isSelected = true;
		//data->isShowDebug = false;
		data->pickedData.name = mPickedRitem->name;
		data->pickedData.isVisible = mPickedRitem->Visible;
		data->pickedData.matName = mPickedRitem->Mat->Name;
		data->pickedData.geoName = mPickedRitem->SubmeshName;
		data->pickedData.layerNum = min((int)mPickedRitem->mLayer, 3);
		for (int i = 0; i < 3; i++)
		{
			data->pickedData.position[i] = mPickedRitem->wTrans[i];
			data->pickedData.scale[i] = mPickedRitem->wScale[i];
			data->pickedData.rotation[i] = mPickedRitem->wRot[i];
			data->pickedData.texscale[i] = mPickedRitem->TexScale[i];
		}

	}
	else
	{
		data->isSelected = false;
	}
}

void RenderItemClass::SetPickedItemOnGui(SelectedObjectDatas data, std::unique_ptr<MaterialClass>* materials, std::unique_ptr<GeoMetryClass>* geometries)
{
	if (mPickedRitem != nullptr)
	{
		mPickedRitem->Visible = data.isVisible;
		mPickedRitem->wTrans = Vector3(data.position);
		mPickedRitem->wScale = Vector3(data.scale);
		mPickedRitem->wRot = Vector3(data.rotation);
		mPickedRitem->TexScale = Vector3(data.texscale);
		mPickedRitem->Mat = materials->get()->GetMaterial(data.matName);
		mPickedRitem->SubmeshName = data.geoName;
		mPickedRitem->Geo = geometries->get()->GetGeometry(geometries->get()->FindParentMesh(data.geoName));
		mPickedRitem->IndexCount = mPickedRitem->Geo->DrawArgs[mPickedRitem->SubmeshName].IndexCount;
		mPickedRitem->StartIndexLocation = mPickedRitem->Geo->DrawArgs[mPickedRitem->SubmeshName].StartIndexLocation;
		mPickedRitem->BaseVertexLocation = mPickedRitem->Geo->DrawArgs[mPickedRitem->SubmeshName].BaseVertexLocation;
		if (data.isLayerChanged)
		{
			if (data.layerNum < 4)
			{
				mRitemLayer[(int)mPickedRitem->mLayer].erase(std::remove(mRitemLayer[(int)mPickedRitem->mLayer].begin(), mRitemLayer[(int)mPickedRitem->mLayer].end(), mPickedRitem), mRitemLayer[(int)mPickedRitem->mLayer].end());
				mRitemLayer[data.layerNum].push_back(mPickedRitem);
				mPickedRitem->mLayer = RenderLayer(data.layerNum);
			}
			data.isLayerChanged = false;
		}
		

		XMStoreFloat4x4(&mPickedRitem->World, MakeMatrixWorld(mPickedRitem->wRot, mPickedRitem->wScale, mPickedRitem->wTrans));
		XMStoreFloat4x4(&mPickedRitem->TexTransform, MakeMatrixTex(mPickedRitem->TexScale));
		mPickedRitem->NumFramesDirty = gNumFrameResources;


		mPickedRitemview->wTrans = Vector3(data.position);
		mPickedRitemview->wScale = Vector3(data.scale);
		mPickedRitemview->wRot = Vector3(data.rotation);
		mPickedRitemview->TexScale = Vector3(data.texscale);
		mPickedRitemview->SubmeshName = data.geoName;
		mPickedRitemview->Geo = geometries->get()->GetGeometry(geometries->get()->FindParentMesh(data.geoName));
		mPickedRitemview->IndexCount = mPickedRitemview->Geo->DrawArgs[mPickedRitemview->SubmeshName].IndexCount;
		mPickedRitemview->StartIndexLocation = mPickedRitemview->Geo->DrawArgs[mPickedRitemview->SubmeshName].StartIndexLocation;
		mPickedRitemview->BaseVertexLocation = mPickedRitemview->Geo->DrawArgs[mPickedRitemview->SubmeshName].BaseVertexLocation;

		XMStoreFloat4x4(&mPickedRitemview->World, MakeMatrixWorld(mPickedRitemview->wRot, mPickedRitemview->wScale, mPickedRitemview->wTrans));
		XMStoreFloat4x4(&mPickedRitemview->TexTransform, MakeMatrixTex(mPickedRitemview->TexScale));
		mPickedRitemview->NumFramesDirty = gNumFrameResources;
	}
}

void RenderItemClass::SaveItemsToJson(ImguiData* data)
{
	mPickedRitem = nullptr;
	mPickedRitemview->Visible = false;
	data->isSelected = false;

	mJson->SaveToJson(mAllRitems);
}

void RenderItemClass::LoadItemsFromJson(std::unique_ptr<MaterialClass>* materials, std::unique_ptr<GeoMetryClass>* geometries, ImguiData* data)
{
	mPickedRitem = nullptr;
	mPickedRitemview->Visible = false;
	data->isSelected = false;

	mAllRitems.clear();
	objCBIndex = 0;
	std::vector<std::shared_ptr<RenderItem>>().swap(mAllRitems);
	for (int i = 0; i < (int)RenderLayer::Count; i++)
	{
		mRitemLayer[i].clear();
		std::vector<RenderItem*>().swap(mRitemLayer[i]);
	}

	mJson->LoadFromJson(mAllRitems, mRitemLayer, materials, geometries, &objCBIndex);
	mPickedRitemview = mRitemLayer[(int)RenderLayer::Highlight][0];
}

void RenderItemClass::AddRenderItem(std::unique_ptr<MaterialClass>* materials, std::unique_ptr<GeoMetryClass>* geometries, std::string name)
{
	auto AddRitem = std::make_unique<RenderItem>();
	AddRitem->name = name;
	AddRitem->wScale = Vector3(1.0f, 1.0f, 1.0f);
	AddRitem->wTrans = Vector3(0.0f, 5.0f, 0.0f);
	XMStoreFloat4x4(&AddRitem->World, MakeMatrixWorld(AddRitem->wRot, AddRitem->wScale, AddRitem->wTrans));
	XMStoreFloat4x4(&AddRitem->TexTransform, MakeMatrixTex(AddRitem->TexScale));
	AddRitem->ObjCBIndex = objCBIndex++;
	AddRitem->Mat = materials->get()->GetMaterial("gray0");
	AddRitem->Geo = geometries->get()->GetGeometry("shapeGeo");
	AddRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	AddRitem->SubmeshName = "box";
	AddRitem->IndexCount = AddRitem->Geo->DrawArgs[AddRitem->SubmeshName].IndexCount;
	AddRitem->StartIndexLocation = AddRitem->Geo->DrawArgs[AddRitem->SubmeshName].StartIndexLocation;
	AddRitem->BaseVertexLocation = AddRitem->Geo->DrawArgs[AddRitem->SubmeshName].BaseVertexLocation;
	AddRitem->mLayer = RenderLayer::Opaque;

	mRitemLayer[(int)AddRitem->mLayer].push_back(AddRitem.get());
	mAllRitems.push_back(std::move(AddRitem));
}

void RenderItemClass::DeleteRenderItem(ImguiData* data)
{
	if (mPickedRitem != nullptr)
	{
		mRitemLayer[(int)mPickedRitem->mLayer].erase(std::remove(mRitemLayer[(int)mPickedRitem->mLayer].begin(), mRitemLayer[(int)mPickedRitem->mLayer].end(), mPickedRitem), mRitemLayer[(int)mPickedRitem->mLayer].end());
		mAllRitems.erase(std::remove_if(mAllRitems.begin(), mAllRitems.end(), [=](const std::shared_ptr<RenderItem>& item) {return item.get() == mPickedRitem; }), mAllRitems.end());

		mPickedRitem = nullptr;
		mPickedRitemview->Visible = false;
		data->isSelected = false;
	}
}

RenderItem* RenderItemClass::GetPickedItem()
{
	if (mPickedRitem != nullptr)
		return mPickedRitem;
	else
		nullptr;
}

RenderItem* RenderItemClass::GetPickedVewItem()
{
	if (mPickedRitem != nullptr)
		return mPickedRitemview;
	else
		nullptr;
}
