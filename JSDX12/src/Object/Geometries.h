#pragma once

#include "d3dUtil.h"
#include "GeometryGenerator.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;


struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

	DirectX::BoundingBox Bounds;
};

struct MeshGeometry
{
	// Give it a name so we can look it up by name.
	std::string Name;

	// System memory copies.  Use Blobs because the vertex/index format can be generic.
	// It is up to the client to cast appropriately.  
	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

	// Data about the buffers.
	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	// A MeshGeometry may store multiple geometries in one vertex/index buffer.
	// Use this container to define the Submesh geometries so we can draw
	// the Submeshes individually.
	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	// We can free this memory after we finish upload to the GPU.
	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexC;
	DirectX::XMFLOAT3 TangentU = DirectX::XMFLOAT3(0, 0, 0);
};

class GeoMetryClass
{
public:
	GeoMetryClass(ID3D12Device* device, ID3D12GraphicsCommandList* commandL);
	GeoMetryClass(const GeoMetryClass& rhs) = delete;
	GeoMetryClass& operator=(const GeoMetryClass& rhs) = delete;
	~GeoMetryClass() = default;
	void BuildGeomatries();
	void BuildParticle(int count);

	MeshGeometry* GetGeometry(std::string str);
	SubmeshGeometry GetSubMesh(std::string str);
	//std::vector<ParticleVertex> GetParicleVertex();
	void SetParticleVertex(std::vector<ParticleVertex> particles);

	std::vector<std::pair<std::string, std::string>> GetSubMeshs();
	void PushSubMesh(std::pair<std::string, std::string> push);
	std::string FindParentMesh(std::string str);
	std::unordered_map<std::string, std::shared_ptr<MeshGeometry>> GetGeos();
	void GeoMetryClass::PushGeos(std::string name, std::shared_ptr<MeshGeometry> mesh);


private:
	ID3D12Device* md3dDevice = nullptr;
	ID3D12GraphicsCommandList* mCommandList = nullptr;
	std::unordered_map<std::string, std::shared_ptr<MeshGeometry>> geometryMap;

	std::vector<std::pair<std::string, std::string>> mSubmeshGeoList;

	//std::vector<ParticleVertex> mParticleVertex;


	void BuildShapeGeometry();
	void BuildTreeSpritesGeometry();
	void BuildParticleGeometry(int count);
	void BuildModelGeometryOri(std::string filename, std::string modelname);
	void BuildModelGeometry(std::string filename, std::string modelname);
};