#include "stdafx.h"
#include "Materials.h"

MaterialClass::MaterialClass()
{
}

void MaterialClass::BuildMaterials()
{
	auto bricks0 = std::make_unique<Material>();
	bricks0->Name = "bricks0";
	bricks0->MatCBIndex = 0;
	bricks0->DiffuseSrvHeapIndex = 3;
	bricks0->NormalSrvHeapIndex = 4;
	bricks0->hasNormalMap = 1;
	bricks0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	bricks0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	bricks0->Roughness = 0.1f;

	auto bricks1 = std::make_unique<Material>();
	bricks1->Name = "bricks1";
	bricks1->MatCBIndex = 1;
	bricks1->DiffuseSrvHeapIndex = 5;
	bricks1->NormalSrvHeapIndex = 6;
	bricks1->hasNormalMap = 1;
	bricks1->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks1->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	bricks1->Roughness = 0.25f;

	auto bricks2 = std::make_unique<Material>();
	bricks2->Name = "bricks2";
	bricks2->MatCBIndex = 2;
	bricks2->DiffuseSrvHeapIndex = 7;
	bricks2->NormalSrvHeapIndex = 2;
	bricks2->hasNormalMap = 0;
	bricks2->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks2->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	bricks2->Roughness = 0.2f;

	auto stone0 = std::make_unique<Material>();
	stone0->Name = "stone0";
	stone0->MatCBIndex = 3;
	stone0->DiffuseSrvHeapIndex = 8;
	stone0->NormalSrvHeapIndex = 2;
	stone0->hasNormalMap = 0;
	stone0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	stone0->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	stone0->Roughness = 0.3f;

	auto tile0 = std::make_unique<Material>();
	tile0->Name = "tile0";
	tile0->MatCBIndex = 4;
	tile0->DiffuseSrvHeapIndex = 9;
	tile0->NormalSrvHeapIndex = 10;
	tile0->hasNormalMap = 1;
	tile0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	tile0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	tile0->Roughness = 0.3f;

	auto checkertile = std::make_unique<Material>();
	checkertile->Name = "checkertile";
	checkertile->MatCBIndex = 5;
	checkertile->DiffuseSrvHeapIndex = 11;
	checkertile->NormalSrvHeapIndex = 2;
	checkertile->hasNormalMap = 0;
	checkertile->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	checkertile->FresnelR0 = XMFLOAT3(0.07f, 0.07f, 0.07f);
	checkertile->Roughness = 0.3f;

	auto icemirror = std::make_unique<Material>();
	icemirror->Name = "icemirror";
	icemirror->MatCBIndex = 6;
	icemirror->DiffuseSrvHeapIndex = 12;
	icemirror->NormalSrvHeapIndex = 2;
	icemirror->hasNormalMap = 0;
	icemirror->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);
	icemirror->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	icemirror->Roughness = 0.5f;

	auto defaultMat = std::make_unique<Material>();
	defaultMat->Name = "default";
	defaultMat->MatCBIndex = 7;
	defaultMat->DiffuseSrvHeapIndex = 1;
	defaultMat->NormalSrvHeapIndex = 2;
	defaultMat->hasNormalMap = 0;
	defaultMat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	defaultMat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	defaultMat->Roughness = 0.3f;


	auto treeSprites = std::make_unique<Material>();
	treeSprites->Name = "treeSprites";
	treeSprites->MatCBIndex = 8;
	treeSprites->DiffuseSrvHeapIndex = 13;
	treeSprites->NormalSrvHeapIndex = 2;
	treeSprites->hasNormalMap = 0;
	treeSprites->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	treeSprites->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	treeSprites->Roughness = 0.125f;


	auto gray0 = std::make_unique<Material>();
	gray0->Name = "gray0";
	gray0->MatCBIndex = 9;
	gray0->DiffuseSrvHeapIndex = 1;
	gray0->NormalSrvHeapIndex = 2;
	gray0->hasNormalMap = 0;
	gray0->DiffuseAlbedo = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	gray0->FresnelR0 = XMFLOAT3(0.04f, 0.04f, 0.04f);
	gray0->Roughness = 0.0f;

	auto highlight0 = std::make_unique<Material>();
	highlight0->Name = "highlight0";
	highlight0->MatCBIndex = 10;
	highlight0->DiffuseSrvHeapIndex = 1;
	highlight0->NormalSrvHeapIndex = 2;
	highlight0->hasNormalMap = 0;
	highlight0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.6f);
	highlight0->FresnelR0 = XMFLOAT3(0.06f, 0.06f, 0.06f);
	highlight0->Roughness = 0.0f;

	auto mirror0 = std::make_unique<Material>();
	mirror0->Name = "mirror0";
	mirror0->MatCBIndex = 11;
	mirror0->DiffuseSrvHeapIndex = 1;
	mirror0->NormalSrvHeapIndex = 2;
	mirror0->hasNormalMap = 0;
	mirror0->DiffuseAlbedo = XMFLOAT4(0.0f, 0.0f, 0.1f, 1.0f);
	mirror0->FresnelR0 = XMFLOAT3(0.98f, 0.97f, 0.95f);
	mirror0->Roughness = 0.1f;

	auto sky = std::make_unique<Material>();
	sky->Name = "sky";
	sky->MatCBIndex = 12;
	sky->DiffuseSrvHeapIndex = 14;
	sky->NormalSrvHeapIndex = 2;
	sky->hasNormalMap = 0;
	sky->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	sky->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	sky->Roughness = 1.0f;

	materialMap["bricks0"] = std::move(bricks0);
	materialMap["bricks1"] = std::move(bricks1);
	materialMap["bricks2"] = std::move(bricks2);
	materialMap["stone0"] = std::move(stone0);
	materialMap["tile0"] = std::move(tile0);
	materialMap["checkertile"] = std::move(checkertile);
	materialMap["icemirror"] = std::move(icemirror);
	materialMap["defaultMat"] = std::move(defaultMat);
	materialMap["treeSprites"] = std::move(treeSprites);
	materialMap["gray0"] = std::move(gray0);
	materialMap["highlight0"] = std::move(highlight0);
	materialMap["mirror0"] = std::move(mirror0);
	materialMap["sky"] = std::move(sky);

}

std::unordered_map<std::string, std::shared_ptr<Material>> MaterialClass::GetMaterials()
{
	return materialMap;
}

Material* MaterialClass::GetMaterial(std::string str)
{
	return materialMap[str].get();
}

UINT MaterialClass::GetMaterialSize()
{
	return materialMap.size();
}
