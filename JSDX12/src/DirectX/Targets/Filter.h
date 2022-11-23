#pragma once
#include "d3dUtil.h"

class Filter
{
public:
	///<summary>
	/// The width and height should match the dimensions of the input texture to apply the filter.
	/// Recreate when the screen is resized. 
	///</summary>
	Filter(ID3D12Device* device,
		UINT width, UINT height,
		DXGI_FORMAT format);

	Filter(const Filter& rhs) = delete;
	Filter& operator=(const Filter& rhs) = delete;
	~Filter() = default;

	CD3DX12_GPU_DESCRIPTOR_HANDLE OutputSrv();

	UINT DescriptorCount()const;

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
		UINT descriptorSize);

	void OnResize(UINT newWidth, UINT newHeight);

	void Execute(
		ID3D12GraphicsCommandList* cmdList,
		ID3D12RootSignature* rootSig,
		ID3D12PipelineState* pso,
		CD3DX12_GPU_DESCRIPTOR_HANDLE input);

private:
	void BuildDescriptors();
	void BuildResource();

private:

	ID3D12Device* md3dDevice = nullptr;

	UINT mWidth = 0;
	UINT mHeight = 0;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuUav;

	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuUav;

	Microsoft::WRL::ComPtr<ID3D12Resource> mOutput = nullptr;
};

