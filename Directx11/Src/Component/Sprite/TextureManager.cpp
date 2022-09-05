#include "stdafx.h"
#include "Texture.h"
#include "TextureManager.h"

TextureManager::TextureManager()
{
	textureCount = 0;
}

TextureManager::~TextureManager()
{
}

void TextureManager::AddTexture(ID3D11Device* device, const WCHAR* filename, TextureType type)
{
	Texture *texture = new Texture();
	if (device == nullptr)
	{
		return;
	}
	texture->Initialize(device, filename);
	//pair<Texture*, TextureType> temp = make_pair(texture, type);
	m_textures.push_back(make_pair(texture, type));
	textureCount++;
}

ID3D11ShaderResourceView* TextureManager::GetTexture(TextureType type)
{
	for (auto& texture : m_textures)
		if (texture.second == type) return texture.first->GetTexture();
	return nullptr;
}

void TextureManager::ShutDown()
{
	for (auto  &texture : m_textures)
		delete texture.first;
	textureCount = 0;
}
