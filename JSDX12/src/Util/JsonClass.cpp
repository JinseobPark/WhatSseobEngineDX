#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "JsonClass.h"

using namespace std;

void JsonClass::JsonTest()
{
	Json::Value root;
	Json::Value string_value;

	for (int i = 0; i < 3; i++)
		root["integer_key"].append(i + 1);

	string_value["string"][0] = "value1";
	string_value["string"][1] = "value2";
	string_value["string"][2] = "value3";
	
	string_value["cstring"][0] = "cvalue1";
	string_value["cstring"][1] = "cvalue2";
	string_value["cstring"][2] = "cvalue3";

	root["string_key"] = (string_value); //append 사용가능
	ofstream outFile("Resources/test.json", ios::out);
	outFile << root;
}

void JsonClass::JsonReadTest()
{
	Json::Value root;
	std::ifstream ifs;
	ifs.open("Resources/test.json");

	Json::CharReaderBuilder builder;
	builder["collectComments"] = true;
	JSONCPP_STRING errs;
	if (!parseFromStream(builder, ifs, &root, &errs)) {
		std::cout << errs << std::endl;
		return;
	}
	std::cout << root["integer_key"] << std::endl;
}

void JsonClass::SaveToJson(std::vector<std::shared_ptr<RenderItem>> items)
{
	Json::Value root;
	Json::Value string_value;
	std::string item_name;
	 RenderItem* temp;
	for (auto  item : items)
	{
		temp = item.get();
		root[temp->name]["ID"] = temp->ObjCBIndex;

		root[temp->name]["Rotate"].append(temp->wRot.x);
		root[temp->name]["Rotate"].append(temp->wRot.y);
		root[temp->name]["Rotate"].append(temp->wRot.z);

		root[temp->name]["Scale"].append(temp->wScale.x);
		root[temp->name]["Scale"].append(temp->wScale.y);
		root[temp->name]["Scale"].append(temp->wScale.z);

		root[temp->name]["Transform"].append(temp->wTrans.x);
		root[temp->name]["Transform"].append(temp->wTrans.y);
		root[temp->name]["Transform"].append(temp->wTrans.z);

		root[temp->name]["TexScale"].append(temp->TexScale.x);
		root[temp->name]["TexScale"].append(temp->TexScale.y);
		root[temp->name]["TexScale"].append(temp->TexScale.z);

		root[temp->name]["Visible"] = temp->Visible;
		root[temp->name]["Mat"] = temp->Mat->Name;
		root[temp->name]["Geo"] = temp->Geo->Name;
		root[temp->name]["SubGeo"] = temp->SubmeshName;
		root[temp->name]["Layer"] = static_cast<int>(temp->mLayer);
		root[temp->name]["Name"] = temp->name;


	}
	ofstream outFile("Resources/objects.json", ios::out);
	outFile << root;
}

void JsonClass::LoadFromJson(std::vector<std::shared_ptr<RenderItem>>& items, std::vector<RenderItem*> layers[], std::unique_ptr<MaterialClass>* materials, std::unique_ptr<GeoMetryClass>* geometries, UINT* lastID)
{

	Json::Value root;
	std::ifstream ifs;
	ifs.open("Resources/objects.json");

	Json::CharReaderBuilder builder;
	builder["collectComments"] = true;
	JSONCPP_STRING errs;
	if (!parseFromStream(builder, ifs, &root, &errs)) {
		std::cout << errs << std::endl;
		return;
	}

	for (auto item : root)
	{
		auto anItem = std::make_unique<RenderItem>();

		anItem->name = item["Name"].asString();
		anItem->wRot = Vector3(item["Rotate"][0].asFloat(), item["Rotate"][1].asFloat(), item["Rotate"][2].asFloat());
		anItem->wScale= Vector3(item["Scale"][0].asFloat(), item["Scale"][1].asFloat(), item["Scale"][2].asFloat());
		anItem->wTrans = Vector3(item["Transform"][0].asFloat(), item["Transform"][1].asFloat(), item["Transform"][2].asFloat());
		anItem->TexScale = Vector3(item["TexScale"][0].asFloat(), item["TexScale"][1].asFloat(), item["TexScale"][2].asFloat());
		anItem->Geo = geometries->get()->GetGeometry(item["Geo"].asString());
		anItem->Mat = materials->get()->GetMaterial(item["Mat"].asString());
		anItem->SubmeshName = item["SubGeo"].asString();
		anItem->Visible = item["Visible"].asBool();
		anItem->mLayer = static_cast<RenderLayer>(item["Layer"].asInt());
		anItem->ObjCBIndex = (*lastID)++;

		anItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		if(anItem->SubmeshName == "points" || anItem->SubmeshName == "particle") anItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		anItem->IndexCount = anItem->Geo->DrawArgs[anItem->SubmeshName].IndexCount;
		anItem->StartIndexLocation = anItem->Geo->DrawArgs[anItem->SubmeshName].StartIndexLocation;
		anItem->BaseVertexLocation = anItem->Geo->DrawArgs[anItem->SubmeshName].BaseVertexLocation;
		XMStoreFloat4x4(&anItem->World, MakeMatrixWorld(anItem->wRot, anItem->wScale, anItem->wTrans));
		XMStoreFloat4x4(&anItem->TexTransform, MakeMatrixTex(anItem->TexScale));

		layers[(int)anItem->mLayer].push_back(anItem.get());
		items.push_back(std::move(anItem));
		std::cout << item << std::endl;
	}


}


