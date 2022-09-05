#include "stdafx.h"
#include "Texture.h"
#include "Systems/Graphics/Graphics.h"
#include "Systems/Graphics/D3DClass.h"


Texture::Texture()
{
}


Texture::Texture(const Texture& other)
{
}


Texture::~Texture()
{
}


bool Texture::Initialize(ID3D11Device* device, const WCHAR* filename)
{
	// read dds texture
	if (FAILED(CreateDDSTextureFromFile(device, filename, nullptr, &m_texture)))
	{
		return false;
	}
	
	return true;
}



void Texture::Shutdown()
{
	// release texture
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_texture;
}
