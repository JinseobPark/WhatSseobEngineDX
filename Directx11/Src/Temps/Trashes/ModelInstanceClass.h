#pragma once

class TextureClass;

class ModelInstanceClass : public AlignedAllocationPolicy<16>
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

	struct InstanceType
	{
		XMFLOAT3 position;
	};

public:
	ModelInstanceClass();
	ModelInstanceClass(const ModelInstanceClass&);
	~ModelInstanceClass();

	bool Initialize(ID3D11Device*, const WCHAR*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetVertexCount();
	int GetInstanceCount();
	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	bool ChangeInstanceBuffer(ID3D11DeviceContext*, float movement);
private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, const WCHAR*);
	void ReleaseTexture();

	bool LoadModel(char*);
	void ReleaseModel();


private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	ID3D11Buffer* m_instanceBuffer = nullptr;
	int m_vertexCount = 0;
	int m_instanceCount = 0;
	int m_indexCount = 0;
	TextureClass* m_Texture = nullptr;
	ModelType* m_model = nullptr;
};