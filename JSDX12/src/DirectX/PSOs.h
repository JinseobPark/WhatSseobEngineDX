#pragma once
#include "Shaders.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class PSOclass
{
public:
	PSOclass(ID3D12Device* device);
	PSOclass(const PSOclass& rhs) = delete;
	PSOclass& operator=(const PSOclass& rhs) = delete;
	~PSOclass() = default;

	void Init();
	void BuildPSOs();

	ComPtr<ID3D12PipelineState> GetPSO(std::string str);

private:
	ID3D12Device* md3dDevice = nullptr;
	std::unique_ptr<ShaderClass> mShaders;

	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;
};