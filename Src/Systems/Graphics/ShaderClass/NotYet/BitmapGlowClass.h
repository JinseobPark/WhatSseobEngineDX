#pragma once

class Texture;

class BitmapGlowClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	BitmapGlowClass();
	BitmapGlowClass(const BitmapGlowClass&);
	~BitmapGlowClass();

	bool Initialize(ID3D11Device*, int, int, const WCHAR*, const WCHAR*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView* GetGlowMap();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTextures(ID3D11Device*, const WCHAR*, const WCHAR*);
	void ReleaseTextures();

private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;
	Texture* m_Texture = nullptr;
	Texture* m_GlowMap = nullptr;
	int m_screenWidth = 0;
	int m_screenHeight = 0;
	int m_bitmapWidth = 0;
	int m_bitmapHeight = 0;
	int m_previousPosX = 0;
	int m_previousPosY = 0;
};