#pragma once

#include "d3dUtil.h"
#include "../Object/Texture/Materials.h"
#include "../Object/Geometries.h"
#include "FrameResource.h"
#include <string>


inline DirectX::XMMATRIX MakeMatrixWorld(Vector3 rot, Vector3 scale, Vector3 trans)
{
	DirectX::XMMATRIX result;
	result = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixScaling(scale.x, scale.y, scale.z) * XMMatrixTranslation(trans.x, trans.y, trans.z);
	return result;
}

inline DirectX::XMMATRIX MakeMatrixTex(Vector3 scale)
{
	DirectX::XMMATRIX result;
	result = XMMatrixScaling(scale.x, scale.y, scale.z);
	return result;
}



struct RenderItem
{
	RenderItem() = default;
	RenderItem(const RenderItem& rhs) = delete;

	bool Visible = true;

	// World matrix of the shape that describes the object's local space
	// relative to the world space, which defines the position, orientation,
	// and scale of the object in the world.
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
	Vector3 wRot = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 wScale = Vector3(1.0f, 1.0f, 1.0f);
	Vector3 wTrans = Vector3(0, 0, 0);
	
	DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
	Vector3 TexScale = Vector3(1, 1, 1);
	// Dirty flag indicating the object data has changed and we need to update the constant buffer.
	// Because we have an object cbuffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify obect data we should set 
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
	int NumFramesDirty = gNumFrameResources;

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
	UINT ObjCBIndex = -1;

	Material* Mat = nullptr;
	MeshGeometry* Geo = nullptr;
	std::string SubmeshName = "";

	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	DirectX::BoundingBox Bounds;

	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;

	RenderLayer mLayer = RenderLayer::Opaque;

	std::string name = "";
};