#include "stdafx.h"
#include "RenderItems.h"
#include <string>

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

void RenderItemClass::BuildRenderItems(std::unique_ptr<MaterialClass>* materials, std::unique_ptr<GeoMetryClass>* geometries)
{
	UINT objCBIndex = 0;

	auto skyRitem = std::make_unique<RenderItem>();
	skyRitem->name = "skybox";
	skyRitem->wScale = Vector3(5000.0f, 5000.0f, 5000.0f);
	XMStoreFloat4x4(&skyRitem->World, MakeMatrixWorld(skyRitem->wRot, skyRitem->wScale, skyRitem->wTrans));
	XMStoreFloat4x4(&skyRitem->TexTransform, MakeMatrixTex(skyRitem->TexScale));
	//XMStoreFloat4x4(&skyRitem->World, XMMatrixScaling(skyRitem->wScale.x, skyRitem->wScale.y, skyRitem->wScale.z));
	//skyRitem->TexTransform = MathHelper::Identity4x4();
	skyRitem->ObjCBIndex = objCBIndex++;
	skyRitem->Mat = materials->get()->GetMaterial("sky");
	skyRitem->Geo = geometries->get()->GetGeometry("shapeGeo");
	skyRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;	
	skyRitem->SubmeshName = "sphere";
	skyRitem->IndexCount = skyRitem->Geo->DrawArgs[skyRitem->SubmeshName].IndexCount;
	skyRitem->StartIndexLocation = skyRitem->Geo->DrawArgs[skyRitem->SubmeshName].StartIndexLocation;
	skyRitem->BaseVertexLocation = skyRitem->Geo->DrawArgs[skyRitem->SubmeshName].BaseVertexLocation;
	skyRitem->mLayer = RenderLayer::Sky;

	mRitemLayer[(int)skyRitem->mLayer].push_back(skyRitem.get());
	mAllRitems.push_back(std::move(skyRitem));

	auto quadRitem = std::make_unique<RenderItem>();
	quadRitem->name = "shadowDebugWindow";
	XMStoreFloat4x4(&quadRitem->World, MakeMatrixWorld(quadRitem->wRot, quadRitem->wScale, quadRitem->wTrans));
	XMStoreFloat4x4(&quadRitem->TexTransform, MakeMatrixTex(quadRitem->TexScale));
	//quadRitem->World = MathHelper::Identity4x4();
	//quadRitem->TexTransform = MathHelper::Identity4x4();
	quadRitem->ObjCBIndex = objCBIndex++;
	quadRitem->Mat = materials->get()->GetMaterial("bricks0");
	quadRitem->Geo = geometries->get()->GetGeometry("shapeGeo");
	quadRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	quadRitem->SubmeshName = "shadow_quad";
	quadRitem->IndexCount = quadRitem->Geo->DrawArgs[quadRitem->SubmeshName].IndexCount;
	quadRitem->StartIndexLocation = quadRitem->Geo->DrawArgs[quadRitem->SubmeshName].StartIndexLocation;
	quadRitem->BaseVertexLocation = quadRitem->Geo->DrawArgs[quadRitem->SubmeshName].BaseVertexLocation;
	quadRitem->mLayer = RenderLayer::ShadowDebug;

	mRitemLayer[(int)quadRitem->mLayer].push_back(quadRitem.get());
	mAllRitems.push_back(std::move(quadRitem));

	auto quadRitem2 = std::make_unique<RenderItem>();
	quadRitem2->name = "ssaoDebugWindow";
	XMStoreFloat4x4(&quadRitem2->World, MakeMatrixWorld(quadRitem2->wRot, quadRitem2->wScale, quadRitem2->wTrans));
	XMStoreFloat4x4(&quadRitem2->TexTransform, MakeMatrixTex(quadRitem2->TexScale));
	//quadRitem2->World = MathHelper::Identity4x4();
	//quadRitem2->TexTransform = MathHelper::Identity4x4();
	quadRitem2->ObjCBIndex = objCBIndex++;
	quadRitem2->Mat = materials->get()->GetMaterial("bricks0");
	quadRitem2->Geo = geometries->get()->GetGeometry("shapeGeo");
	quadRitem2->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	quadRitem2->SubmeshName = "ssao_quad";
	quadRitem2->IndexCount = quadRitem2->Geo->DrawArgs[quadRitem2->SubmeshName].IndexCount;
	quadRitem2->StartIndexLocation = quadRitem2->Geo->DrawArgs[quadRitem2->SubmeshName].StartIndexLocation;
	quadRitem2->BaseVertexLocation = quadRitem2->Geo->DrawArgs[quadRitem2->SubmeshName].BaseVertexLocation;
	quadRitem2->mLayer = RenderLayer::SsaoDebug;

	mRitemLayer[(int)quadRitem2->mLayer].push_back(quadRitem2.get());
	mAllRitems.push_back(std::move(quadRitem2));

	auto boxRitem = std::make_unique<RenderItem>();
	boxRitem->name = "centerbox";
	boxRitem->wScale = Vector3(2.0f, 2.0f, 2.0f);
	boxRitem->wTrans = Vector3(0.0f, 1.2f, 0.0f);
	XMStoreFloat4x4(&boxRitem->World, MakeMatrixWorld(boxRitem->wRot, boxRitem->wScale, boxRitem->wTrans));
	XMStoreFloat4x4(&boxRitem->TexTransform, MakeMatrixTex(boxRitem->TexScale));
	//XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 1.2f, 0.0f));
	//XMStoreFloat4x4(&boxRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	boxRitem->ObjCBIndex = objCBIndex++;
	boxRitem->Mat = materials->get()->GetMaterial("mirror0");
	boxRitem->Geo = geometries->get()->GetGeometry("shapeGeo");
	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->SubmeshName = "box";
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs[boxRitem->SubmeshName].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs[boxRitem->SubmeshName].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs[boxRitem->SubmeshName].BaseVertexLocation;
	boxRitem->mLayer = RenderLayer::Opaque;

	mRitemLayer[(int)boxRitem->mLayer].push_back(boxRitem.get());
	mAllRitems.push_back(std::move(boxRitem));

	auto gridRitem = std::make_unique<RenderItem>();
	gridRitem->name = "grid";
	gridRitem->wScale = Vector3(20.0f, 0.1f, 30.0f);
	gridRitem->TexScale = Vector3(8.0f, 8.0f, 1.0f);
	XMStoreFloat4x4(&gridRitem->World, MakeMatrixWorld(gridRitem->wRot, gridRitem->wScale, gridRitem->wTrans));
	XMStoreFloat4x4(&gridRitem->TexTransform, MakeMatrixTex(gridRitem->TexScale));
	//XMStoreFloat4x4(&gridRitem->World, XMMatrixScaling(20.0f, 0.1f, 30.0f));
	//XMStoreFloat4x4(&gridRitem->TexTransform, XMMatrixScaling(8.0f, 8.0f, 1.0f));
	gridRitem->ObjCBIndex = objCBIndex++;
	gridRitem->Mat = materials->get()->GetMaterial("tile0");
	gridRitem->Geo = geometries->get()->GetGeometry("shapeGeo");
	gridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->SubmeshName = "grid";
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs[gridRitem->SubmeshName].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs[gridRitem->SubmeshName].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs[gridRitem->SubmeshName].BaseVertexLocation;
	gridRitem->mLayer = RenderLayer::Opaque;

	mRitemLayer[(int)gridRitem->mLayer].push_back(gridRitem.get());
	mAllRitems.push_back(std::move(gridRitem));

	XMMATRIX brickTexTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 5; ++i)
	{
		auto leftCylRitem = std::make_unique<RenderItem>();
		auto rightCylRitem = std::make_unique<RenderItem>();
		auto leftSphereRitem = std::make_unique<RenderItem>();
		auto rightSphereRitem = std::make_unique<RenderItem>();

		XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

		XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

		leftCylRitem->name = "leftCy" + std::to_string(i);
		leftCylRitem->wTrans = Vector3(-5.0f, 1.5f, -10.0f + i * 5.0f);
		XMStoreFloat4x4(&leftCylRitem->World, MakeMatrixWorld(leftCylRitem->wRot, leftCylRitem->wScale, leftCylRitem->wTrans));
		XMStoreFloat4x4(&leftCylRitem->TexTransform, MakeMatrixTex(leftCylRitem->TexScale));
		//XMStoreFloat4x4(&leftCylRitem->World, rightCylWorld);
		//XMStoreFloat4x4(&leftCylRitem->TexTransform, brickTexTransform);
		leftCylRitem->ObjCBIndex = objCBIndex++;
		leftCylRitem->Mat = materials->get()->GetMaterial("bricks1");
		leftCylRitem->Geo = geometries->get()->GetGeometry("shapeGeo");
		leftCylRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylRitem->SubmeshName = "cylinder";
		leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs[leftCylRitem->SubmeshName].IndexCount;
		leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs[leftCylRitem->SubmeshName].StartIndexLocation;
		leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs[leftCylRitem->SubmeshName].BaseVertexLocation;
		leftCylRitem->mLayer = RenderLayer::Opaque;

		rightCylRitem->name = "rightCy" + std::to_string(i);
		rightCylRitem->wTrans = Vector3(+5.0f, 1.5f, -10.0f + i * 5.0f);
		XMStoreFloat4x4(&rightCylRitem->World, MakeMatrixWorld(rightCylRitem->wRot, rightCylRitem->wScale, rightCylRitem->wTrans));
		XMStoreFloat4x4(&rightCylRitem->TexTransform, MakeMatrixTex(rightCylRitem->TexScale));
		//XMStoreFloat4x4(&rightCylRitem->World, leftCylWorld);
		//XMStoreFloat4x4(&rightCylRitem->TexTransform, brickTexTransform);
		rightCylRitem->ObjCBIndex = objCBIndex++;
		rightCylRitem->Mat = materials->get()->GetMaterial("bricks0");
		rightCylRitem->Geo = geometries->get()->GetGeometry("shapeGeo");
		rightCylRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylRitem->SubmeshName = "cylinder";
		rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs[rightCylRitem->SubmeshName].IndexCount;
		rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs[rightCylRitem->SubmeshName].StartIndexLocation;
		rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs[rightCylRitem->SubmeshName].BaseVertexLocation;
		rightCylRitem->mLayer = RenderLayer::Opaque;


		leftSphereRitem->name = "leftSphere" + std::to_string(i);
		leftSphereRitem->wTrans = Vector3(-5.0f, 3.5f, -10.0f + i * 5.0f);
		XMStoreFloat4x4(&leftSphereRitem->World, MakeMatrixWorld(leftSphereRitem->wRot, leftSphereRitem->wScale, leftSphereRitem->wTrans));
		XMStoreFloat4x4(&leftSphereRitem->TexTransform, MakeMatrixTex(leftSphereRitem->TexScale));
		//XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
		//leftSphereRitem->TexTransform = MathHelper::Identity4x4();
		leftSphereRitem->ObjCBIndex = objCBIndex++;
		leftSphereRitem->Mat = materials->get()->GetMaterial("stone0");
		leftSphereRitem->Geo = geometries->get()->GetGeometry("shapeGeo");
		leftSphereRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRitem->SubmeshName = "sphere";
		leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs[leftSphereRitem->SubmeshName].IndexCount;
		leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs[leftSphereRitem->SubmeshName].StartIndexLocation;
		leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs[leftSphereRitem->SubmeshName].BaseVertexLocation;
		leftSphereRitem->mLayer = RenderLayer::Transparent;

		rightSphereRitem->name = "rightSphere" + std::to_string(i);
		rightSphereRitem->wTrans = Vector3(+5.0f, 3.5f, -10.0f + i * 5.0f);
		XMStoreFloat4x4(&rightSphereRitem->World, MakeMatrixWorld(rightSphereRitem->wRot, rightSphereRitem->wScale, rightSphereRitem->wTrans));
		XMStoreFloat4x4(&rightSphereRitem->TexTransform, MakeMatrixTex(rightSphereRitem->TexScale));
		//XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
		//rightSphereRitem->TexTransform = MathHelper::Identity4x4();
		rightSphereRitem->ObjCBIndex = objCBIndex++;
		rightSphereRitem->Mat = materials->get()->GetMaterial("mirror0");
		rightSphereRitem->Geo = geometries->get()->GetGeometry("shapeGeo");
		rightSphereRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightSphereRitem->SubmeshName = "sphere";
		rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs[rightSphereRitem->SubmeshName].IndexCount;
		rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs[rightSphereRitem->SubmeshName].StartIndexLocation;
		rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs[rightSphereRitem->SubmeshName].BaseVertexLocation;
		rightSphereRitem->mLayer = RenderLayer::Opaque;

		mRitemLayer[(int)leftCylRitem->mLayer].push_back(leftCylRitem.get());
		mRitemLayer[(int)rightCylRitem->mLayer].push_back(rightCylRitem.get());
		mRitemLayer[(int)leftSphereRitem->mLayer].push_back(leftSphereRitem.get());
		mRitemLayer[(int)rightSphereRitem->mLayer].push_back(rightSphereRitem.get());

		//mRitemLayer[(int)RenderLayer::Opaque].push_back(leftCylRitem.get());
		//mRitemLayer[(int)RenderLayer::Opaque].push_back(rightCylRitem.get());
		//mRitemLayer[(int)RenderLayer::Transparent].push_back(leftSphereRitem.get());
		//mRitemLayer[(int)RenderLayer::Opaque].push_back(rightSphereRitem.get());

		mAllRitems.push_back(std::move(leftCylRitem));
		mAllRitems.push_back(std::move(rightCylRitem));
		mAllRitems.push_back(std::move(leftSphereRitem));
		mAllRitems.push_back(std::move(rightSphereRitem));
	}

	auto treeSpritesRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&treeSpritesRitem->World, MakeMatrixWorld(treeSpritesRitem->wRot, treeSpritesRitem->wScale, treeSpritesRitem->wTrans));
	XMStoreFloat4x4(&treeSpritesRitem->TexTransform, MakeMatrixTex(treeSpritesRitem->TexScale));
	//treeSpritesRitem->World = MathHelper::Identity4x4();
	treeSpritesRitem->name = "tree";
	treeSpritesRitem->ObjCBIndex = objCBIndex++;
	treeSpritesRitem->Mat = materials->get()->GetMaterial("tile0");
	treeSpritesRitem->Geo = geometries->get()->GetGeometry("treeSpritesGeo");
	treeSpritesRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	treeSpritesRitem->SubmeshName = "points";
	treeSpritesRitem->IndexCount = treeSpritesRitem->Geo->DrawArgs[treeSpritesRitem->SubmeshName].IndexCount;
	treeSpritesRitem->StartIndexLocation = treeSpritesRitem->Geo->DrawArgs[treeSpritesRitem->SubmeshName].StartIndexLocation;
	treeSpritesRitem->BaseVertexLocation = treeSpritesRitem->Geo->DrawArgs[treeSpritesRitem->SubmeshName].BaseVertexLocation;
	treeSpritesRitem->mLayer = RenderLayer::AlphaTestedTreeSprites;

	mRitemLayer[(int)treeSpritesRitem->mLayer].push_back(treeSpritesRitem.get());
	mAllRitems.push_back(std::move(treeSpritesRitem));

	//
	// car
	//

	auto carRitem2 = std::make_unique<RenderItem>();
	carRitem2->wTrans = Vector3(-5.0f, 2.0f, 0.0f);
	carRitem2->TexScale = Vector3(4.0f, 4.0f, 1.0f);
	XMStoreFloat4x4(&carRitem2->World, MakeMatrixWorld(carRitem2->wRot, carRitem2->wScale, carRitem2->wTrans));
	XMStoreFloat4x4(&carRitem2->TexTransform, MakeMatrixTex(carRitem2->TexScale));
	//XMStoreFloat4x4(&carRitem2->World, XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(-5.0f, 2.0f, 0.0f));
	//XMStoreFloat4x4(&carRitem2->TexTransform, XMMatrixScaling(4.0f, 4.0f, 1.0f));
	carRitem2->name = "car2";
	carRitem2->ObjCBIndex = objCBIndex++;
	carRitem2->Mat = materials->get()->GetMaterial("tile0");
	carRitem2->Geo = geometries->get()->GetGeometry("carGeo");
	carRitem2->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	carRitem2->SubmeshName = "car";
	carRitem2->IndexCount = carRitem2->Geo->DrawArgs[carRitem2->SubmeshName].IndexCount;
	carRitem2->StartIndexLocation = carRitem2->Geo->DrawArgs[carRitem2->SubmeshName].StartIndexLocation;
	carRitem2->BaseVertexLocation = carRitem2->Geo->DrawArgs[carRitem2->SubmeshName].BaseVertexLocation;
	carRitem2->Bounds = carRitem2->Geo->DrawArgs[carRitem2->SubmeshName].Bounds;
	carRitem2->mLayer = RenderLayer::Opaque;

	mRitemLayer[(int)carRitem2->mLayer].push_back(carRitem2.get());
	mAllRitems.push_back(std::move(carRitem2));

	auto carRitem = std::make_unique<RenderItem>();
	carRitem->wTrans = Vector3(0.0f, 5.0f, 0.0f);
	carRitem->TexScale = Vector3(4.0f, 4.0f, 1.0f);
	XMStoreFloat4x4(&carRitem->World, MakeMatrixWorld(carRitem->wRot, carRitem->wScale, carRitem->wTrans));
	XMStoreFloat4x4(&carRitem->TexTransform, MakeMatrixTex(carRitem->TexScale));
	//XMStoreFloat4x4(&carRitem->World, XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0.0f, 5.0f, 0.0f));
	//XMStoreFloat4x4(&carRitem->TexTransform, XMMatrixScaling(4.0f, 4.0f, 1.0f));
	carRitem->name = "car1";
	carRitem->ObjCBIndex = objCBIndex++;
	carRitem->Mat = materials->get()->GetMaterial("mirror0");
	carRitem->Geo = geometries->get()->GetGeometry("carGeo");
	carRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	carRitem->SubmeshName = "car";
	carRitem->IndexCount = carRitem->Geo->DrawArgs[carRitem->SubmeshName].IndexCount;
	carRitem->StartIndexLocation = carRitem->Geo->DrawArgs[carRitem->SubmeshName].StartIndexLocation;
	carRitem->BaseVertexLocation = carRitem->Geo->DrawArgs[carRitem->SubmeshName].BaseVertexLocation;
	carRitem->mLayer = RenderLayer::Opaque;

	mRitemLayer[(int)carRitem->mLayer].push_back(carRitem.get());
	mAllRitems.push_back(std::move(carRitem));


	auto mirrorRitem = std::make_unique<RenderItem>();
	mirrorRitem->wTrans = Vector3(4.0f, 4.0f, 0.0f);
	mirrorRitem->TexScale = Vector3(4.0f, 4.0f, 1.0f);
	XMStoreFloat4x4(&mirrorRitem->World, MakeMatrixWorld(mirrorRitem->wRot, mirrorRitem->wScale, mirrorRitem->wTrans));
	XMStoreFloat4x4(&mirrorRitem->TexTransform, MakeMatrixTex(mirrorRitem->TexScale));
	//XMStoreFloat4x4(&mirrorRitem->World, XMMatrixScaling(1.0f, 1.0f, 1.0f)* XMMatrixTranslation(4.0f, 4.0f, 0.0f));
	//XMStoreFloat4x4(&mirrorRitem->TexTransform, XMMatrixScaling(4.0f, 4.0f, 1.0f));
	mirrorRitem->name = "mirror";
	mirrorRitem->ObjCBIndex = objCBIndex++;
	mirrorRitem->Mat = materials->get()->GetMaterial("mirror0");
	mirrorRitem->Geo = geometries->get()->GetGeometry("shapeGeo");
	mirrorRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mirrorRitem->SubmeshName = "sphere";
	mirrorRitem->IndexCount = mirrorRitem->Geo->DrawArgs[mirrorRitem->SubmeshName].IndexCount;
	mirrorRitem->StartIndexLocation = mirrorRitem->Geo->DrawArgs[mirrorRitem->SubmeshName].StartIndexLocation;
	mirrorRitem->BaseVertexLocation = mirrorRitem->Geo->DrawArgs[mirrorRitem->SubmeshName].BaseVertexLocation;
	mirrorRitem->mLayer = RenderLayer::Opaque;

	mRitemLayer[(int)mirrorRitem->mLayer].push_back(mirrorRitem.get());
	mAllRitems.push_back(std::move(mirrorRitem));

	//
	// picked item
	//
	auto pickedRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&pickedRitem->World, MakeMatrixWorld(pickedRitem->wRot, pickedRitem->wScale, pickedRitem->wTrans));
	XMStoreFloat4x4(&pickedRitem->TexTransform, MakeMatrixTex(pickedRitem->TexScale));
	pickedRitem->name = "pickitem";
	pickedRitem->World = MathHelper::Identity4x4();
	pickedRitem->TexTransform = MathHelper::Identity4x4();
	pickedRitem->ObjCBIndex = objCBIndex++;
	pickedRitem->Mat = materials->get()->GetMaterial("highlight0");
	pickedRitem->Geo = geometries->get()->GetGeometry("carGeo");
	pickedRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Picked triangle is not visible until one is picked.
	pickedRitem->Visible = false;

	// DrawCall parameters are filled out when a triangle is picked.
	pickedRitem->IndexCount = 0;
	pickedRitem->StartIndexLocation = 0;
	pickedRitem->BaseVertexLocation = 0;
	mPickedRitemview = pickedRitem.get();
	pickedRitem->mLayer = RenderLayer::Highlight;

	mRitemLayer[(int)pickedRitem->mLayer].push_back(pickedRitem.get());
	mAllRitems.push_back(std::move(pickedRitem));

	mJson->SaveToJson(mAllRitems);
}

void RenderItemClass::BuildRenderItemsFromJson(std::unique_ptr<MaterialClass>* materials, std::unique_ptr<GeoMetryClass>* geometries)
{
	mJson->LoadFromJson(mAllRitems, mRitemLayer, materials, geometries);
	mPickedRitemview = mRitemLayer[(int)RenderLayer::Highlight][0];
}

//void RenderItemClass::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems, FrameResource* curFrames)
//{
//	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
//
//	auto objectCB = curFrames->ObjectCB->Resource();
//
//	// For each render item...
//	for (size_t i = 0; i < ritems.size(); ++i)
//	{
//		auto ri = ritems[i];
//
//		if (ri->Visible == false)
//			continue;
//
//		cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
//		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
//		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);
//
//		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;
//
//		cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);
//
//		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
//	}
//}

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

void RenderItemClass::Pick(int sx, int sy, Camera camera, int widht, int height)
{
	bool isPass = false;
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
	imguidata.isSelected = false;

	float tmax = 65536.0f;
	// Check if we picked an opaque render item.  A real app might keep a separate "picking list"
	// of objects that can be selected.   
	for (auto ri : mRitemLayer[(int)RenderLayer::Opaque])
	{
		//auto geo = ri->Geo;

		// Skip invisible render-items.
		if (ri->Visible == false)
			continue;

		XMMATRIX W = XMLoadFloat4x4(&ri->World);
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

		//rayOrigin = XMVector3TransformCoord(rayOrigin, invWorld);
		//rayDir = XMVector3TransformNormal(rayDir, invWorld);

		//// Tranform ray to vi space of Mesh.
		//XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

		XMVECTOR newrayOrigin = XMVector3TransformCoord(rayOrigin, invWorld);
		XMVECTOR newrayDir = XMVector3TransformNormal(rayDir, invWorld);

		// Make the ray direction unit length for the intersection tests.
		newrayDir = XMVector3Normalize(newrayDir);

		// If we hit the bounding box of the Mesh, then we might have picked a Mesh triangle,
		// so do the ray/triangle tests.
		//
		// If we did not hit the bounding box, then it is impossible that we hit 
		// the Mesh, so do not waste effort doing ray/triangle tests.
		float distance = 0.0f;
		if (ri->Bounds.Intersects(newrayOrigin, newrayDir, distance))
		{
			if (distance < tmax)
			{
				tmax = distance;

				mPickedRitemview->Visible = true;

				// Picked render item needs same world matrix as object picked.
				mPickedRitemview->Geo = ri->Geo;
				mPickedRitemview->World = ri->World;
				mPickedRitemview->TexTransform = ri->TexTransform;
				mPickedRitemview->NumFramesDirty = gNumFrameResources;

				// Offset to the picked triangle in the mesh index buffer.
				mPickedRitemview->IndexCount = ri->IndexCount;
				mPickedRitemview->BaseVertexLocation = ri->BaseVertexLocation;
				mPickedRitemview->StartIndexLocation = ri->StartIndexLocation;

				mPickedRitem = ri;
				GetPickedItem();
			}

		}
	}


}

void RenderItemClass::GetPickedItem()
{
	if (mPickedRitem != nullptr)
	{
		imguidata.isSelected = true;
		imguidata.isShowDebug = false;
		imguidata.pickedData.isVisible = mPickedRitem->Visible;
	}
	else
	{
		imguidata.isSelected = false;
	}
}

void RenderItemClass::SetPickedItem(SelectedObjectDatas data)
{
	mPickedRitem->Visible = data.isVisible;
	//imguidata.pickedData.isVisible

}
