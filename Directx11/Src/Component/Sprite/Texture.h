#pragma once

class Texture
{
public:
	Texture();
	Texture(const Texture&);
	~Texture();

	bool Initialize(ID3D11Device*, const WCHAR* filename);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();


private:
	ID3D11ShaderResourceView* m_texture = nullptr;
};