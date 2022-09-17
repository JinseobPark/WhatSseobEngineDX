#pragma once

class Texture;

class TreeClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	TreeClass();
	TreeClass(const TreeClass&);
	~TreeClass();

	bool Initialize(ID3D11Device*, const char*, const WCHAR*, const char*, const WCHAR*, float);
	void Shutdown();

	void RenderTrunk(ID3D11DeviceContext*);
	void RenderLeaves(ID3D11DeviceContext*);

	int GetTrunkIndexCount();
	int GetLeafIndexCount();

	ID3D11ShaderResourceView* GetTrunkTexture();
	ID3D11ShaderResourceView* GetLeafTexture();

	void SetPosition(XMFLOAT3);
	void GetPosition(XMFLOAT3&);

private:
	bool InitializeTrunkBuffers(ID3D11Device*, float);
	bool InitializeLeafBuffers(ID3D11Device*, float);

	void ShutdownBuffers();

	void RenderTrunkBuffers(ID3D11DeviceContext*);
	void RenderLeafBuffers(ID3D11DeviceContext*);

	bool LoadTextures(ID3D11Device*, const WCHAR*, const WCHAR*);
	void ReleaseTextures();

	bool LoadModel(const char*);
	void ReleaseModel();

private:
	ID3D11Buffer* m_trunkVertexBuffer = nullptr;
	ID3D11Buffer* m_trunkIndexBuffer = nullptr;
	ID3D11Buffer* m_leafVertexBuffer = nullptr;
	ID3D11Buffer* m_leafIndexBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;
	int m_trunkIndexCount = 0;
	int m_leafIndexCount = 0;
	Texture* m_TrunkTexture = nullptr;
	Texture* m_LeafTexture = nullptr;
	ModelType* m_model = nullptr;
	XMFLOAT3 m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
};