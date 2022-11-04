#pragma once

#include "d3dUtil.h"
#include "../DirectX/DirectXTex.h"
#include "../DirectX/DirectXTex.inl"
//#include "../Object/Object.h"

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_Debug.lib")
#else
#pragma comment(lib, "DirectXTex.lib")
#endif

using Microsoft::WRL::ComPtr;
using namespace DirectX;

struct Texture
{
	// Unique material name for lookup.
	std::string Name;

	std::wstring Filename;

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

class TextureClass
{
public:
	TextureClass(ID3D12Device* device, ID3D12GraphicsCommandList* commandL);
	TextureClass(const TextureClass& rhs) = delete;
	TextureClass& operator=(const TextureClass& rhs) = delete;
	~TextureClass() = default;

	void Init();
	void LoadTextures();

	ComPtr<ID3D12Resource> GetResource(std::string str);
private:
	ID3D12Device* md3dDevice = nullptr;
	ID3D12GraphicsCommandList* mCommandList = nullptr;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextureMap;

};