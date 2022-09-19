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
#include "Texture/CubeRenderTarget.h"
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
	//void UpdateReflectedPassCB(const GameTimer& gt);
	void UpdateCubeMapFacePassCBs();

	void LoadTextures();
	void BuildRootSignature();
	void BuildDescriptorHeaps();
	void BuildCubeDepthStencil();
	void BuildShadersAndInputLayout();
	void BuildShapeGeometry();
	void BuildTreeSpritesGeometry();
	void BuildCarGeometry();

	void BuildPSOs();
	void BuildFrameResources();
	void BuildMaterials();

	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);
	void DrawSceneToCubeMap();

	void Pick(int sx, int sy);
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
	void BuildCubeFaceCamera(float x, float y, float z);


	//Imgui
	void ImguiInitialize();
	void ImguiLoadInfo();
	void ImguiUpdate(const GameTimer& gt);
	void ImguiShutdown();
private:

	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	//UINT mCbvSrvDescriptorSize = 0;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	ComPtr<ID3D12Resource> mCubeDepthStencilBuffer;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mTreeSpriteInputLayout;

	// List of all the render items.
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	// Render items divided by PSO.
	// Render items divided by PSO.
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];


	RenderItem* mPickedRitem = nullptr;

	//UINT mInstanceCount = 0;
	//bool mFrustumCullingEnabled = true;
	UINT mSkyTexHeapIndex = 0;
	UINT mDynamicTexHeapIndex = 0;

	std::unique_ptr<CubeRenderTarget> mDynamicCubeMap = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mCubeDSV;

	PassConstants mMainPassCB;
	//PassConstants mReflectedPassCB;

	//BoundingFrustum mCamFrustum;

	Camera mCamera;
	Camera mCubeMapCamera[6];

	POINT mLastMousePos;

	ImguiData guidata;
};