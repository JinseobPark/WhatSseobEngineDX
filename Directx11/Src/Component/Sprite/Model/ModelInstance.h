#pragma once
#include "../TextureManager.h"
#include "Systems/Graphics/Graphics.h"
#include "Component/Component.h"

class Texture;
class TextureManager;
class Graphics;

class ModelInstance : public Component
{
	typedef unsigned int RenderQueue;
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
		float padding;
	};

public:
	ModelInstance();
	~ModelInstance();

	void Initialize() override;
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetVertexCount();
	int GetInstanceCount();
	int GetIndexCount();

	void AddTransform(Transform *transform);
	void AddTextures(const WCHAR* filename, TextureType type);
	ID3D11ShaderResourceView* GetTexture(TextureType type);

	bool ChangeInstanceBuffer();

	bool ChangeInstanceNewBuffer();

	bool LoadModel(const char*);
private:
	//Initialize
	bool InitializeBuffers();
	void InitializeTextureManager();

	//Release
	void ShutdownBuffers();
	void ReleaseTextures();
	void ReleaseModel();

	//Render
	void RenderBuffers(ID3D11DeviceContext* deviceContext);
private:
	friend class Graphics;

	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	ID3D11Buffer* m_instanceBuffer = nullptr;
	int m_vertexCount = 0;
	int m_instanceCount = 0;
	int m_indexCount = 0;
	ModelType* m_model = nullptr;
	TextureManager* m_textureManager = nullptr;

	InstanceType* m_instances = nullptr;
	vector<Transform*> m_transforms;
	RenderQueue renderqueue;
	Transform* m_transform = nullptr;
};