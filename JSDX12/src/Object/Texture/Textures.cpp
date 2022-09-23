#include "stdafx.h"
#include "Textures.h"

TextureClass::TextureClass(ID3D12Device* device, ID3D12GraphicsCommandList* commandL)
{
	md3dDevice = device;
	mCommandList = commandL;
}

void TextureClass::Init()
{
}

void TextureClass::LoadTextures()
{
	std::vector<std::string> texNames =
	{
		"dump",					//0
		"defaultTex",					//1
		"defaultNormal",			 //2
		"bricksTex",					// 3
		"bricksTexN",			    // 4
		"bricksTex2",				 //5
		"bricksTex2N",			   //6
		"bricksTex3",				  //7
		"stoneTex",					   //8
		"tileTex",					      //9
		"tileTexN",				    //10
		"checkboardTex",			 //11
		"iceTex",						//12
		"treeArrayTex",				//13
		"skyCubeMap"				//14
	};

	std::vector<std::wstring> texFilenames =
	{
		L"Textures/white1x1.dds",			//0
		L"Textures/white1x1.dds",			//1
		L"Textures/default_nmap.dds",	//2
		L"Textures/bricks.dds",				//3
		L"Textures/bricks_nmap.dds",		//4
		L"Textures/bricks2.dds",				//5
		L"Textures/bricks2_nmap.dds",	//6
		L"Textures/bricks3.dds",				//7
		L"Textures/stone.dds",					//8
		L"Textures/tile.dds",						//9
		L"Textures/tile_nmap.dds",			//10
		L"Textures/checkboard.dds",		//11
		L"Textures/ice.dds",						//12
		L"Textures/treeArray2.dds",			//13
		L"Textures/grasscube1024.dds"	//14
	};

	for (int i = 0; i < (int)texNames.size(); ++i)
	{
		auto texMap = std::make_unique<Texture>();
		texMap->Name = texNames[i];
		texMap->Filename = texFilenames[i];
		ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice,
			mCommandList, texMap->Filename.c_str(),
			texMap->Resource, texMap->UploadHeap));

		mTextureMap[texMap->Name] = std::move(texMap);
	}
}

ComPtr<ID3D12Resource> TextureClass::GetResource(std::string str)
{
	return mTextureMap[str]->Resource;
}
