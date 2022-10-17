#include "stdafx.h"
#include "Shaders.h"

ShaderClass::ShaderClass()
{
}

void ShaderClass::BuildShadersAndInputLayout()
{
	const D3D_SHADER_MACRO defines[] =
	{
		"FOG", "1",
		NULL, NULL
	};

	const D3D_SHADER_MACRO alphaTestDefines[] =
	{
		"FOG", "1",
		"ALPHA_TEST", "1",
		NULL, NULL
	};

	shaderMap["standardVS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
	shaderMap["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", defines, "PS", "ps_5_1");
	shaderMap["alphaTestedPS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", alphaTestDefines, "PS", "ps_5_1");

	shaderMap["treeSpriteVS"] = d3dUtil::CompileShader(L"Shaders\\TreeSprite.hlsl", nullptr, "VS", "vs_5_1");
	shaderMap["treeSpriteGS"] = d3dUtil::CompileShader(L"Shaders\\TreeSprite.hlsl", nullptr, "GS", "gs_5_1");
	shaderMap["treeSpritePS"] = d3dUtil::CompileShader(L"Shaders\\TreeSprite.hlsl", alphaTestDefines, "PS", "ps_5_1");

	shaderMap["skyVS"] = d3dUtil::CompileShader(L"Shaders\\Sky.hlsl", nullptr, "VS", "vs_5_1");
	shaderMap["skyPS"] = d3dUtil::CompileShader(L"Shaders\\Sky.hlsl", nullptr, "PS", "ps_5_1");

	shaderMap["shadowVS"] = d3dUtil::CompileShader(L"Shaders\\Shadows.hlsl", nullptr, "VS", "vs_5_1");
	shaderMap["shadowOpaquePS"] = d3dUtil::CompileShader(L"Shaders\\Shadows.hlsl", nullptr, "PS", "ps_5_1");
	shaderMap["shadowAlphaTestedPS"] = d3dUtil::CompileShader(L"Shaders\\Shadows.hlsl", alphaTestDefines, "PS", "ps_5_1");

	shaderMap["debugshadowVS"] = d3dUtil::CompileShader(L"Shaders\\ShadowDebug.hlsl", nullptr, "VS", "vs_5_1");
	shaderMap["debugshadowPS"] = d3dUtil::CompileShader(L"Shaders\\ShadowDebug.hlsl", nullptr, "PS", "ps_5_1");

	shaderMap["debugSsaoVS"] = d3dUtil::CompileShader(L"Shaders\\SsaoDebug.hlsl", nullptr, "VS", "vs_5_1");
	shaderMap["debugSsaoPS"] = d3dUtil::CompileShader(L"Shaders\\SsaoDebug.hlsl", nullptr, "PS", "ps_5_1");

	shaderMap["drawNormalsVS"] = d3dUtil::CompileShader(L"Shaders\\DrawNormals.hlsl", nullptr, "VS", "vs_5_1");
	shaderMap["drawNormalsPS"] = d3dUtil::CompileShader(L"Shaders\\DrawNormals.hlsl", nullptr, "PS", "ps_5_1");
	
	shaderMap["ssaoVS"] = d3dUtil::CompileShader(L"Shaders\\Ssao.hlsl", nullptr, "VS", "vs_5_1");
	shaderMap["ssaoPS"] = d3dUtil::CompileShader(L"Shaders\\Ssao.hlsl", nullptr, "PS", "ps_5_1");
	shaderMap["ssaoBlurVS"] = d3dUtil::CompileShader(L"Shaders\\SsaoBlur.hlsl", nullptr, "VS", "vs_5_1");
	shaderMap["ssaoBlurPS"] = d3dUtil::CompileShader(L"Shaders\\SsaoBlur.hlsl", nullptr, "PS", "ps_5_1");
	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	mTreeSpriteInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

ComPtr<ID3DBlob> ShaderClass::GetShader(std::string str)
{
	return shaderMap[str].Get();
}

std::vector<D3D12_INPUT_ELEMENT_DESC> ShaderClass::GetInputLayout()
{
	return mInputLayout;
}

std::vector<D3D12_INPUT_ELEMENT_DESC> ShaderClass::GetTreeSpriteInputLayout()
{
	return mTreeSpriteInputLayout;
}
