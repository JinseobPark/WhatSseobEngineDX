#pragma once

#include "stdafx.h"
#include "MathHelper.h"
#include "FrameResource.h"
//#include "d3dUtil.h"

//extern const int gNumFrameResources;

// Defines a subrange of geometry in a MeshGeometry.  This is for when multiple
// geometries are stored in one vertex and index buffer.  It provides the offsets
// and data needed to draw a subset of geometry stores in the vertex and index 
// buffers so that we can implement the technique described by Figure 6.3.
//struct SubmeshGeometry
//{
//	UINT IndexCount = 0;
//	UINT StartIndexLocation = 0;
//	INT BaseVertexLocation = 0;
//
//	// Bounding box of the geometry defined by this submesh. 
//	// This is used in later chapters of the book.
//	DirectX::BoundingBox Bounds;
//};
//
//struct MeshGeometry
//{
//	// Give it a name so we can look it up by name.
//	std::string Name;
//
//	// System memory copies.  Use Blobs because the vertex/index format can be generic.
//	// It is up to the client to cast appropriately.  
//	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
//	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;
//
//	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
//	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;
//
//	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
//	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;
//
//	// Data about the buffers.
//	UINT VertexByteStride = 0;
//	UINT VertexBufferByteSize = 0;
//	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
//	UINT IndexBufferByteSize = 0;
//
//	// A MeshGeometry may store multiple geometries in one vertex/index buffer.
//	// Use this container to define the Submesh geometries so we can draw
//	// the Submeshes individually.
//	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;
//
//	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
//	{
//		D3D12_VERTEX_BUFFER_VIEW vbv;
//		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
//		vbv.StrideInBytes = VertexByteStride;
//		vbv.SizeInBytes = VertexBufferByteSize;
//
//		return vbv;
//	}
//
//	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
//	{
//		D3D12_INDEX_BUFFER_VIEW ibv;
//		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
//		ibv.Format = IndexFormat;
//		ibv.SizeInBytes = IndexBufferByteSize;
//
//		return ibv;
//	}
//
//	// We can free this memory after we finish upload to the GPU.
//	void DisposeUploaders()
//	{
//		VertexBufferUploader = nullptr;
//		IndexBufferUploader = nullptr;
//	}
//};

struct Light
{
	DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
	float FalloffStart = 1.0f;                          // point/spot light only
	DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
	float FalloffEnd = 10.0f;                           // point/spot light only
	DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
	float SpotPower = 64.0f;                            // spot light only
};


struct MaterialConstants
{
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.25f;

	// Used in texture mapping.
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};

// Simple struct to represent a material for our demos.  A production 3D engine
// would likely create a class hierarchy of Materials.
//struct Material
//{
//	// Unique material name for lookup.
//	std::string Name;
//
//	// Index into constant buffer corresponding to this material.
//	int MatCBIndex = -1;
//
//	// Index into SRV heap for diffuse texture.
//	int DiffuseSrvHeapIndex = -1;
//
//	// Index into SRV heap for normal texture.
//	int NormalSrvHeapIndex = -1;
//	int hasNormalMap = 0;
//
//	// Dirty flag indicating the material has changed and we need to update the constant buffer.
//	// Because we have a material constant buffer for each FrameResource, we have to apply the
//	// update to each FrameResource.  Thus, when we modify a material we should set 
//	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
//	int NumFramesDirty = gNumFrameResources;
//
//	// Material constant buffer data used for shading.
//	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
//	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
//	float Roughness = .25f;
//	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
//};

//struct Texture
//{
//	// Unique material name for lookup.
//	std::string Name;
//
//	std::wstring Filename;
//
//	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
//	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
//};

enum class RenderLayer : int
{
	Opaque = 0,
	Transparent,
	OpaqueDynamicReflectors, 
	AlphaTested,
	AlphaTestedTreeSprites,
	Mirrors,
	Reflected,
	Shadow,
	Highlight,
	Sky,
	ShadowDebug,
	SsaoDebug,
	Count
};

//std::vector<std::string> LayerMap = { "Opaque", "Transparent", "AlphaTested",  "AlphaTestedTreeSprites",  "Mirrors",  "Shadow",  "Highlight",  "Sky", "OpaqueDynamicReflectors",  "ShadowDebug",  "SsaoDebug",  "Count" };
//std::string mymap[] = {"hi", "bi"};
//std::map<RenderLayer, std::string> LayerMaps = {
//	{RenderLayer::Opaque, "Opaque"},
//	{RenderLayer::Transparent, "Transparent"},
//	{RenderLayer::OpaqueDynamicReflectors, "OpaqueDynamicReflectors"},
//	{RenderLayer::Mirrors, "Mirrors"}
//};

//struct RenderItem
//{
//	RenderItem() = default;
//	RenderItem(const RenderItem & rhs) = delete;
//
//	// World matrix of the shape that describes the object's local space
//	// relative to the world space, which defines the position, orientation,
//	// and scale of the object in the world.
//	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
//
//	DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
//	// Dirty flag indicating the object data has changed and we need to update the constant buffer.
//	// Because we have an object cbuffer for each FrameResource, we have to apply the
//	// update to each FrameResource.  Thus, when we modify obect data we should set 
//	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
//	int NumFramesDirty = gNumFrameResources;
//
//	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
//	UINT ObjCBIndex = -1;
//
//	Material* Mat = nullptr;
//	MeshGeometry* Geo = nullptr;
//
//	// Primitive topology.
//	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//
//	DirectX::BoundingBox Bounds;
//	std::vector<InstanceData> Instances;
//
//	// DrawIndexedInstanced parameters.
//	UINT IndexCount = 0;
//	UINT InstanceCount = 0;
//	UINT StartIndexLocation = 0;
//	int BaseVertexLocation = 0;
//};