#pragma once
#include "d3dApp.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
#include "Object/RenderItem.h"
#include "Object/GeometryGenerator.h"
#include "FrameResource.h"
#include "Object/Object.h"
#include "Object/Camera.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx12.h"
#include "Imgui/imgui_impl_win32.h"
#include "Targets/CubeRenderTarget.h"
#include "Targets/ShadowMap.h"
#include "Targets/Ssao.h"
#include "Object/Texture/Textures.h"
#include "Object/Texture/Materials.h"
#include "Object/Geometries.h"
#include "Object/RenderItems.h"
#include "Shaders.h"
#include "Datas.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;


class Engine : public D3DApp
{
public:
	Engine(HINSTANCE hInstance);
	Engine(const Engine& rhs) = delete;
	Engine& operator=(const Engine& rhs) = delete;
	~Engine();

	virtual bool Initialize()override;

private:
	virtual void CreateRtvAndDsvDescriptorHeaps()override;
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void OnKeyboardInput(const GameTimer& gt);
	void AnimateMaterials(const GameTimer& gt);
	void UpdateObjectCBs(const GameTimer& gt);
	void UpdateMaterialCBs(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);
	//void UpdateCubeMapFacePassCBs();
	void UpdateShadowTransform(const GameTimer& gt);
	void UpdateShadowPassCB(const GameTimer& gt);
	void UpdateSsaoCB(const GameTimer& gt);
	//void UpdateReflectedPassCB(const GameTimer& gt);

	void BuildRootSignature();
	void BuildSsaoRootSignature();
	void BuildDescriptorHeaps();
	//void BuildCubeDepthStencil();

	void BuildPSOs();
	void BuildFrameResources();

	//void BuildRenderItems();
	//void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);
	//void DrawSceneToCubeMap();
	void DrawSceneToShadowMap();
	void DrawNormalsAndDepth();

	//void Pick(int sx, int sy);
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();
	//void BuildCubeFaceCamera(float x, float y, float z);

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuSrv(int index)const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuSrv(int index)const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsv(int index)const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtv(int index)const;

	//Imgui
	void ImguiInitialize();
	void ImguiLoadInfo();
	void ImguiUpdate(const GameTimer& gt);
	void ImguiShutdown();
private:

	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;


	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12RootSignature> mSsaoRootSignature = nullptr;

	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	ComPtr<ID3D12Resource> mCubeDepthStencilBuffer;

	//std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

	std::unique_ptr<GeoMetryClass> mGeometries;
	std::unique_ptr<TextureClass> mTextures;
	std::unique_ptr<MaterialClass> mMaterials;
	std::unique_ptr<ShaderClass> mShaders;

	std::unique_ptr<RenderItemClass> mRenderItems;

	//// List of all the render items.
	//std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	//// Render items divided by PSO.
	//std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];


	RenderItem* mPickedRitem = nullptr;

	//UINT mInstanceCount = 0;
	//bool mFrustumCullingEnabled = true;
	UINT mSkyTexHeapIndex = 0;
	UINT mShadowMapHeapIndex = 0;
	UINT mSsaoHeapIndexStart = 0;
	UINT mSsaoAmbientMapIndex = 0;

	UINT mNullCubeSrvIndex = 0;
	UINT mNullTexSrvIndex1 = 0;
	UINT mNullTexSrvIndex2 = 0;

	CD3DX12_GPU_DESCRIPTOR_HANDLE mNullSrv;

	//About Dynamic cube
	//UINT mDynamicTexHeapIndex = 0;
	//std::unique_ptr<CubeRenderTarget> mDynamicCubeMap = nullptr;
	//CD3DX12_CPU_DESCRIPTOR_HANDLE mCubeDSV;



	PassConstants mMainPassCB;  // index 0 of pass cbuffer.
	PassConstants mShadowPassCB;// index 1 of pass cbuffer.
	//PassConstants mReflectedPassCB;

	//BoundingFrustum mCamFrustum;

	std::unique_ptr<ShadowMap> mShadowMap;
	std::unique_ptr<Ssao> mSsao;

	DirectX::BoundingSphere mSceneBounds;

	float mLightNearZ = 0.0f;
	float mLightFarZ = 0.0f;
	XMFLOAT3 mLightPosW;
	XMFLOAT4X4 mLightView = MathHelper::Identity4x4();
	XMFLOAT4X4 mLightProj = MathHelper::Identity4x4();
	XMFLOAT4X4 mShadowTransform = MathHelper::Identity4x4();

	float mLightRotationAngle = 0.0f;
	XMFLOAT3 mBaseLightDirections[3] = {
		XMFLOAT3(0.57735f, -0.57735f, 0.57735f),
		XMFLOAT3(-0.57735f, -0.57735f, 0.57735f),
		XMFLOAT3(0.0f, -0.707f, -0.707f)
	};
	XMFLOAT3 mRotatedLightDirections[3];

	Camera mCamera;
	//Camera mCubeMapCamera[6];

	POINT mLastMousePos;

	ImguiData guidata;
};