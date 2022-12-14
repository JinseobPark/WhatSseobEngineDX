#pragma once
#include "json/json/json.h"
#include "../Object/RenderItem.h"

class JsonClass
{
public:
	JsonClass() {}
	void JsonTest();
	~JsonClass() = default;

	void JsonReadTest();

	void SaveToJson(std::vector<std::shared_ptr<RenderItem>> items);
	void LoadFromJson(std::vector<std::shared_ptr<RenderItem>>& items, std::vector<RenderItem*> layers[], std::unique_ptr<MaterialClass>* materials, std::unique_ptr<GeoMetryClass>* geometries, UINT *lastID);

private:
	//Json::Value root;
};