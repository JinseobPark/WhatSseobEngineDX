#pragma once
#include "../TextureManager.h"
#include "Systems/Graphics/Graphics.h"
#include "Component/Component.h"

class Texture;
class TextureManager;
class Graphics;

class ModelBillboard : public Component
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
	ModelBillboard();
	~ModelBillboard();

	void Initialize() override;
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();
	void AddTextures(const WCHAR* filename, TextureType type);
	ID3D11ShaderResourceView* GetTexture(TextureType type);

	bool LoadModel(const char*);
private:
	//Initialize
	bool InitializeBuffers();
	void InitializeTextureManager();

	//Relase
	void ShutdownBuffers();
	void ReleaseTextures();
	void ReleaseModel();

	//Render
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
	friend class Graphics;

	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;
	ModelType* m_model = nullptr;
	TextureManager* m_textureManager = nullptr;

	Transform* m_transform = nullptr;
};