#pragma once
#include "RenderItem.h"
#include "Camera.h"
#include "JsonClass.h"
#include "../Datas.h"

class JsonClass;

class RenderItemClass
{
public:
	RenderItemClass();
	RenderItemClass(const RenderItemClass& rhs) = delete;
	RenderItemClass& operator=(const RenderItemClass& rhs) = delete;
	~RenderItemClass();


	void BuildRenderItems(std::unique_ptr<MaterialClass>* materials, std::unique_ptr<GeoMetryClass>* geometries);
	void BuildRenderItemsFromJson(std::unique_ptr<MaterialClass>* materials, std::unique_ptr<GeoMetryClass>* geometries);
	//void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems, FrameResource* curFrames);
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, int layer, FrameResource* curFrames);

	std::vector<std::shared_ptr<RenderItem>> GetAllRiems();

	void Pick(int sx, int sy, Camera camera, int widht, int height, ImguiData *data);

	void GetPickedItemOnGui(ImguiData* data);

	void SetPickedItemOnGui(SelectedObjectDatas data, std::unique_ptr<MaterialClass>* materials, std::unique_ptr<GeoMetryClass>* geometries);

	void SaveItemsToJson(ImguiData* data);
	void LoadItemsFromJson(std::unique_ptr<MaterialClass>* materials, std::unique_ptr<GeoMetryClass>* geometries, ImguiData* data);

	void AddRenderItem(std::unique_ptr<MaterialClass>* materials, std::unique_ptr<GeoMetryClass>* geometries, std::string name);

	void DeleteRenderItem(ImguiData* data);

	RenderItem* GetPickedItem();
	RenderItem* GetPickedVewItem();


private:
	UINT objCBIndex = 0;

	// List of all the render items.
	std::vector<std::shared_ptr<RenderItem>> mAllRitems;

	// Render items divided by PSO.
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];

	RenderItem* mPickedRitemview = nullptr;
	RenderItem* mPickedRitem = nullptr;


	JsonClass* mJson = nullptr;
};