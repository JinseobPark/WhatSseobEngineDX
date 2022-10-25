#pragma once

#include "d3dUtil.h"
//#include "../Object/Object.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

// Simple struct to represent a material for our demos.  A production 3D engine
// would likely create a class hierarchy of Materials.
struct Material
{
	// Unique material name for lookup.
	std::string Name;

	// Index into constant buffer corresponding to this material.
	int MatCBIndex = -1;

	// Index into SRV heap for diffuse texture.
	int DiffuseSrvHeapIndex = -1;

	// Index into SRV heap for normal texture.
	int NormalSrvHeapIndex = -1;
	int hasNormalMap = 0;

	// Dirty flag indicating the material has changed and we need to update the constant buffer.
	// Because we have a material constant buffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify a material we should set 
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
	int NumFramesDirty = gNumFrameResources;

	// Material constant buffer data used for shading.
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = .25f;
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};


class MaterialClass
{
public:
	MaterialClass();
	MaterialClass(const MaterialClass& rhs) = delete;
	MaterialClass& operator=(const MaterialClass& rhs) = delete;
	~MaterialClass() = default;

	void BuildMaterials();
	std::unordered_map<std::string, std::shared_ptr<Material>> GetMaterials();

	Material* GetMaterial(std::string);
	UINT GetMaterialSize();

	std::vector<std::string> GetMaterialstrings();

private:
	//ID3D12Device* md3dDevice = nullptr;
	//ID3D12GraphicsCommandList* mCommandList = nullptr;
	std::unordered_map<std::string, std::shared_ptr<Material>> materialMap;

};