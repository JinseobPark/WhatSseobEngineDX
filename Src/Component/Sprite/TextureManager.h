#pragma once
#include <vector>
#include <algorithm>

using namespace std;

//Texture Types
enum TextureType
{
	DEFAULT,
	NORMAL,
	SPECULAR
};

//extern TextureType mTextureType;

class Texture;
class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	void AddTexture(ID3D11Device*, const WCHAR* filename, TextureType type);
	ID3D11ShaderResourceView* GetTexture(TextureType type);
	void ShutDown();

	//Pairing texture and type
	vector<pair<Texture*, TextureType>> m_textures;
private:
	int textureCount;
};