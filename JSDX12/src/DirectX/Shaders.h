#pragma once
#include "d3dUtil.h"


using Microsoft::WRL::ComPtr;
using namespace DirectX;

class ShaderClass
{
public :
	ShaderClass();
	ShaderClass(const ShaderClass& rhs) = delete;
	ShaderClass& operator=(const ShaderClass& rhs) = delete;
	~ShaderClass() = default;

	void BuildShadersAndInputLayout();

	ComPtr<ID3DBlob> GetShader(std::string str);

	std::vector<D3D12_INPUT_ELEMENT_DESC> GetInputLayout();
	std::vector<D3D12_INPUT_ELEMENT_DESC> GetOutlineInputLayout();
	std::vector<D3D12_INPUT_ELEMENT_DESC> GetTreeSpriteInputLayout();

private:
	std::unordered_map<std::string, ComPtr<ID3DBlob>> shaderMap;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mOutlineInputLayout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mTreeSpriteInputLayout;

};