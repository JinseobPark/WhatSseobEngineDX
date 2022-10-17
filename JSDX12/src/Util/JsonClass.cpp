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
	//Json::Value root;
	//Json::Reader reader;
	//ifstream json("Resources/test.json", ifstream::binary);

	//reader.parse(json, root);

	//for (auto& value : root)
	//	cout << value << endl;

	//for (auto& value : root["string_key"])
	//	cout << value.asString() << endl;

	//for (auto& value : root["integer_key"])
	//	cout << value.asInt() << endl;

	//cout << root.get("string_key", "UTF-8");


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

void JsonClass::LoadFromJson(std::vector<std::shared_ptr<RenderItem>>& items, std::vector<RenderItem*> layers[], std::unique_ptr<MaterialClass>* materials, std::unique_ptr<GeoMetryClass>* geometries)
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
		anItem->ObjCBIndex = item["ID"].asInt();

		anItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		if(anItem->SubmeshName == "points") anItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		anItem->IndexCount = anItem->Geo->DrawArgs[anItem->SubmeshName].IndexCount;
		anItem->StartIndexLocation = anItem->Geo->DrawArgs[anItem->SubmeshName].StartIndexLocation;
		anItem->BaseVertexLocation = anItem->Geo->DrawArgs[anItem->SubmeshName].BaseVertexLocation;
		XMStoreFloat4x4(&anItem->World, MakeMatrixWorld(anItem->wRot, anItem->wScale, anItem->wTrans));
		XMStoreFloat4x4(&anItem->TexTransform, MakeMatrixTex(anItem->TexScale));

		layers[(int)anItem->mLayer].push_back(anItem.get());
		items.push_back(std::move(anItem));
		std::cout << item << std::endl;
	}


	/*
	
	auto skyRitem = std::make_unique<RenderItem>();
	skyRitem->name = "skybox";
	skyRitem->wScale = Vector3(5000.0f, 5000.0f, 5000.0f);
	XMStoreFloat4x4(&skyRitem->World, MakeMatrixWorld(skyRitem->wRot, skyRitem->wScale, skyRitem->wTrans));
	XMStoreFloat4x4(&skyRitem->TexTransform, MakeMatrixTex(skyRitem->TexScale));
	//XMStoreFloat4x4(&skyRitem->World, XMMatrixScaling(skyRitem->wScale.x, skyRitem->wScale.y, skyRitem->wScale.z));
	//skyRitem->TexTransform = MathHelper::Identity4x4();
	skyRitem->ObjCBIndex = objCBIndex++;
	skyRitem->Mat = materials->get()->GetMaterial("sky");
	skyRitem->Geo = geometries->get()->GetGeometry("shapeGeo");
	skyRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;	
	skyRitem->Geo->SubmeshName = "sphere";
	skyRitem->IndexCount = skyRitem->Geo->DrawArgs[skyRitem->Geo->SubmeshName].IndexCount;
	skyRitem->StartIndexLocation = skyRitem->Geo->DrawArgs[skyRitem->Geo->SubmeshName].StartIndexLocation;
	skyRitem->BaseVertexLocation = skyRitem->Geo->DrawArgs[skyRitem->Geo->SubmeshName].BaseVertexLocation;
	skyRitem->mLayer = RenderLayer::Sky;

	mRitemLayer[(int)skyRitem->mLayer].push_back(skyRitem.get());
	mAllRitems.push_back(std::move(skyRitem));
	*/
}


