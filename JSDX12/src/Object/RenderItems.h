#pragma once
#include "RenderItem.h"
#include "Camera.h"

class RenderItemClass
{
public:
	RenderItemClass();
	RenderItemClass(const RenderItemClass& rhs) = delete;
	RenderItemClass& operator=(const RenderItemClass& rhs) = delete;
	~RenderItemClass() = default;


	void BuildRenderItems(std::unique_ptr<MaterialClass>* materials, std::unique_ptr<GeoMetryClass>* geometries);
	//void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems, FrameResource* curFrames);
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, int layer, FrameResource* curFrames);

	std::vector<std::shared_ptr<RenderItem>> GetAllRiems();

	void Pick(int sx, int sy, Camera camera, int widht, int height);
private:

	// List of all the render items.
	std::vector<std::shared_ptr<RenderItem>> mAllRitems;

	// Render items divided by PSO.
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];

	RenderItem* mPickedRitem = nullptr;
};