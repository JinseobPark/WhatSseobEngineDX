#include "stdafx.h"
#include "Geometries.h"
#include "Logger.h"
#include <time.h>

GeoMetryClass::GeoMetryClass(ID3D12Device* device, ID3D12GraphicsCommandList* commandL)
{
	md3dDevice = device;
	mCommandList = commandL;
}

void GeoMetryClass::BuildGeomatries()
{
    Logger time_loger(LOG_LEVEL_INFO);
    clock_t temp;
    std::string timers;
    temp = clock();
	BuildShapeGeometry();
    BuildModelGeometryOri("Models/skull.txt", "skull");
    BuildModelGeometryOri("Models/car.txt", "car");
    BuildModelGeometry("Models/dragon2.obj", "dragon2");
    BuildModelGeometry("Models/mirrorball.obj", "mirrorball");
    BuildModelGeometry("Models/spot.obj", "spot");
    BuildModelGeometry("Models/house.obj", "house");
    BuildModelGeometry("Models/light.obj", "light");
    BuildModelGeometry("Models/light2.obj", "light2");
    BuildModelGeometry("Models/light3.obj", "light3");
    BuildModelGeometry("Models/bridge.obj", "bridge");
    BuildModelGeometry("Models/airplane.obj", "airplane");
    BuildModelGeometry("Models/air_building1.obj", "air_building1");
    BuildModelGeometry("Models/air_building2.obj", "air_building2");
    BuildModelGeometry("Models/air_bus.obj", "air_bus");
    BuildModelGeometry("Models/biplane.obj", "biplane");
    BuildModelGeometry("Models/plaques.obj", "plaques");
    BuildModelGeometry("Models/loader.obj", "loader");
    BuildModelGeometry("Models/helicopter.obj", "helicopter");
    BuildModelGeometry("Models/helicopter_place.obj", "helicopter_place");
    BuildModelGeometry("Models/control_tower.obj", "control_tower");
    BuildModelGeometry("Models/hangar1.obj", "hangar1");
    BuildModelGeometry("Models/boarding_ladder.obj", "boarding_ladder");
    BuildTreeSpritesGeometry();
    timers = "Load Model time : " + std::to_string((double)(clock() - temp) / CLOCKS_PER_SEC);
    time_loger.info(timers.c_str(), 1);

}

MeshGeometry* GeoMetryClass::GetGeometry(std::string str)
{
	return geometryMap[str].get();
}

SubmeshGeometry GeoMetryClass::GetSubMesh(std::string str)
{
	for (auto item : geometryMap)
	{
		auto submesh = item.second->DrawArgs.find(str);
		if (submesh != item.second->DrawArgs.end())
		{
			return submesh->second;
		}
	}
    //can not find return box
	return geometryMap["shapeGeo"].get()->DrawArgs["box"];
}

std::vector<std::pair<std::string, std::string>> GeoMetryClass::GetSubMeshs()
{
	return mSubmeshGeoList;
}

std::string GeoMetryClass::FindParentMesh(std::string str)
{
	for (auto item : mSubmeshGeoList)
		if (item.first == str) return item.second;
	return "shapeGeo";
}

std::unordered_map<std::string, std::shared_ptr<MeshGeometry>> GeoMetryClass::GetGeos()
{
	return geometryMap;
}

// Frank Luna Function
void GeoMetryClass::BuildShapeGeometry()
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(1.0f, 1.0f, 60, 40);
	GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);
	GeometryGenerator::MeshData shadow_quad = geoGen.CreateQuad(-1.0f, -0.5f, 0.5f, 0.5f, 0.0f);
	GeometryGenerator::MeshData ssao_quad = geoGen.CreateQuad(-0.5f, -0.5f, 0.5f, 0.5f, 0.0f);

	XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);
	//
	// We are concatenating all the geometry into one big vertex/index buffer.  So
	// define the regions in the buffer each submesh covers.
	//

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();
	UINT shadow_quadVertexOffset = cylinderVertexOffset + (UINT)cylinder.Vertices.size();
	UINT ssao_quadVertexOffset = shadow_quadVertexOffset + (UINT)shadow_quad.Vertices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.Indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();
	UINT shadow_quadIndexOffset = cylinderIndexOffset + (UINT)cylinder.Indices32.size();
	UINT ssao_quadIndexOffset = shadow_quadIndexOffset + (UINT)shadow_quad.Indices32.size();

	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;

	SubmeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	SubmeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	SubmeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

	SubmeshGeometry shadow_quadSubmesh;
	shadow_quadSubmesh.IndexCount = (UINT)shadow_quad.Indices32.size();
	shadow_quadSubmesh.StartIndexLocation = shadow_quadIndexOffset;
	shadow_quadSubmesh.BaseVertexLocation = shadow_quadVertexOffset;

	SubmeshGeometry ssao_quadSubmesh;
	ssao_quadSubmesh.IndexCount = (UINT)ssao_quad.Indices32.size();
	ssao_quadSubmesh.StartIndexLocation = ssao_quadIndexOffset;
	ssao_quadSubmesh.BaseVertexLocation = ssao_quadVertexOffset;
	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	auto totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size() +
		shadow_quad.Vertices.size() +
		ssao_quad.Vertices.size();

	std::vector<Vertex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].TexC = box.Vertices[i].TexC;
		vertices[k].TangentU = box.Vertices[i].TangentU;

		XMVECTOR P = XMLoadFloat3(&vertices[k].Pos);
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}
	XMStoreFloat3(&boxSubmesh.Bounds.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&boxSubmesh.Bounds.Extents, 0.5f * (vMax - vMin));
	vMin = XMLoadFloat3(&vMinf3);
	vMax = XMLoadFloat3(&vMaxf3);

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].TexC = grid.Vertices[i].TexC;
		vertices[k].TangentU = grid.Vertices[i].TangentU;

		XMVECTOR P = XMLoadFloat3(&vertices[k].Pos);
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}
	XMStoreFloat3(&gridSubmesh.Bounds.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&gridSubmesh.Bounds.Extents, 0.5f * (vMax - vMin));
	vMin = XMLoadFloat3(&vMinf3);
	vMax = XMLoadFloat3(&vMaxf3);

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
		vertices[k].TexC = sphere.Vertices[i].TexC;
		vertices[k].TangentU = sphere.Vertices[i].TangentU;

		XMVECTOR P = XMLoadFloat3(&vertices[k].Pos);
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}
	XMStoreFloat3(&sphereSubmesh.Bounds.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&sphereSubmesh.Bounds.Extents, 0.5f * (vMax - vMin));
	vMin = XMLoadFloat3(&vMinf3);
	vMax = XMLoadFloat3(&vMaxf3);

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
		vertices[k].TexC = cylinder.Vertices[i].TexC;
		vertices[k].TangentU = cylinder.Vertices[i].TangentU;

		XMVECTOR P = XMLoadFloat3(&vertices[k].Pos);
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}
	XMStoreFloat3(&cylinderSubmesh.Bounds.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&cylinderSubmesh.Bounds.Extents, 0.5f * (vMax - vMin));
	vMin = XMLoadFloat3(&vMinf3);
	vMax = XMLoadFloat3(&vMaxf3);

	for (int i = 0; i < shadow_quad.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = shadow_quad.Vertices[i].Position;
		vertices[k].Normal = shadow_quad.Vertices[i].Normal;
		vertices[k].TexC = shadow_quad.Vertices[i].TexC;
		vertices[k].TangentU = shadow_quad.Vertices[i].TangentU;

		XMVECTOR P = XMLoadFloat3(&vertices[k].Pos);
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}
	XMStoreFloat3(&shadow_quadSubmesh.Bounds.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&shadow_quadSubmesh.Bounds.Extents, 0.5f * (vMax - vMin));
	vMin = XMLoadFloat3(&vMinf3);
	vMax = XMLoadFloat3(&vMaxf3);

	for (int i = 0; i < ssao_quad.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = ssao_quad.Vertices[i].Position;
		vertices[k].Normal = ssao_quad.Vertices[i].Normal;
		vertices[k].TexC = ssao_quad.Vertices[i].TexC;
		vertices[k].TangentU = ssao_quad.Vertices[i].TangentU;

		XMVECTOR P = XMLoadFloat3(&vertices[k].Pos);
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}
	XMStoreFloat3(&ssao_quadSubmesh.Bounds.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&ssao_quadSubmesh.Bounds.Extents, 0.5f * (vMax - vMin));


	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));
	indices.insert(indices.end(), std::begin(shadow_quad.GetIndices16()), std::end(shadow_quad.GetIndices16()));
	indices.insert(indices.end(), std::begin(ssao_quad.GetIndices16()), std::end(ssao_quad.GetIndices16()));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "shapeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
		mCommandList, vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
		mCommandList, indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;
	geo->DrawArgs["shadow_quad"] = shadow_quadSubmesh;
	geo->DrawArgs["ssao_quad"] = ssao_quadSubmesh;
	mSubmeshGeoList.push_back(std::make_pair("box", "shapeGeo"));
	mSubmeshGeoList.push_back(std::make_pair("grid", "shapeGeo"));
	mSubmeshGeoList.push_back(std::make_pair("sphere", "shapeGeo"));
	mSubmeshGeoList.push_back(std::make_pair("cylinder", "shapeGeo"));
	mSubmeshGeoList.push_back(std::make_pair("shadow_quad", "shapeGeo"));
	mSubmeshGeoList.push_back(std::make_pair("ssao_quad", "shapeGeo"));

	geometryMap[geo->Name] = std::move(geo);
}

void GeoMetryClass::BuildTreeSpritesGeometry()
{
	struct TreeSpriteVertex
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Size;
	};

	static const int treeCount = 40;
	std::array<TreeSpriteVertex, treeCount> vertices;
	for (UINT i = 0; i < treeCount; ++i)
	{
		float x = 30.0f + MathHelper::RandF(-4.0f, 4.0f);
		float z = 30.0f + MathHelper::RandF(-4.0f, 4.0f);
		float y = 0.0f;

		// Move tree slightly above land height.
		y += 1.0f;

		vertices[i].Pos = XMFLOAT3(x, y, z);
		vertices[i].Size = XMFLOAT2(2.0f, 2.0f);
	}

	std::array<std::uint16_t, 40> indices =
	{
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29, 30, 31,
		32, 33, 34, 35, 36, 37, 38, 39
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(TreeSpriteVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "treeSpritesGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
		mCommandList, vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
		mCommandList, indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(TreeSpriteVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["points"] = submesh;
	mSubmeshGeoList.push_back(std::make_pair("points", "treeSpritesGeo"));

	geometryMap["treeSpritesGeo"] = std::move(geo);
}

void GeoMetryClass::BuildModelGeometryOri(std::string filename, std::string modelname)
{

    std::ifstream fin(filename);

    if (!fin)
    {
        MessageBox(0, L"a model not found.", 0, 0);
        return;
    }

    UINT vcount = 0;
    UINT tcount = 0;
    std::string ignore;

    fin >> ignore >> vcount;
    fin >> ignore >> tcount;
    fin >> ignore >> ignore >> ignore >> ignore;

    XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
    XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

    XMVECTOR vMin = XMLoadFloat3(&vMinf3);
    XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

    std::vector<Vertex> vertices(vcount);
    for (UINT i = 0; i < vcount; ++i)
    {
        fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
        fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;

        vertices[i].TangentU = XMFLOAT3(0, 0, 0);
        XMVECTOR P = XMLoadFloat3(&vertices[i].Pos);

        // Project point onto unit sphere and generate spherical texture coordinates.
        XMFLOAT3 spherePos;
        XMStoreFloat3(&spherePos, XMVector3Normalize(P));

        float theta = atan2f(spherePos.z, spherePos.x);

        // Put in [0, 2pi].
        if (theta < 0.0f)
            theta += XM_2PI;

        float phi = acosf(spherePos.y);

        float u = theta / (2.0f * XM_PI);
        float v = phi / XM_PI;

        vertices[i].TexC = { u, v };

        vMin = XMVectorMin(vMin, P);
        vMax = XMVectorMax(vMax, P);
    }

    BoundingBox bounds;
    XMStoreFloat3(&bounds.Center, 0.5f * (vMin + vMax));
    XMStoreFloat3(&bounds.Extents, 0.5f * (vMax - vMin));

    fin >> ignore;
    fin >> ignore;
    fin >> ignore;

    std::vector<std::int32_t> indices(3 * tcount);
    for (UINT i = 0; i < tcount; ++i)
    {
        fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
    }

    fin.close();

    //
    // Pack the indices of all the meshes into one index buffer.
    //

    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);

    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::int32_t);

    auto geo = std::make_unique<MeshGeometry>();
    geo->Name = modelname + "Geo";

    ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
    CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
    CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
        mCommandList, vertices.data(), vbByteSize, geo->VertexBufferUploader);

    geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
        mCommandList, indices.data(), ibByteSize, geo->IndexBufferUploader);

    geo->VertexByteStride = sizeof(Vertex);
    geo->VertexBufferByteSize = vbByteSize;
    geo->IndexFormat = DXGI_FORMAT_R32_UINT;
    geo->IndexBufferByteSize = ibByteSize;

    SubmeshGeometry submesh;
    submesh.IndexCount = (UINT)indices.size();
    submesh.StartIndexLocation = 0;
    submesh.BaseVertexLocation = 0;
    submesh.Bounds = bounds;

    geo->DrawArgs[modelname] = submesh;
    mSubmeshGeoList.push_back(std::make_pair(modelname, geo->Name));

    geometryMap[geo->Name] = std::move(geo);
}

void GeoMetryClass::BuildModelGeometry(std::string filename, std::string modelname)
{
        HRESULT hr = 0;

        std::wifstream fileIn(filename.c_str());    //Open file
        std::wstring meshMatLib;                    //String to hold our obj material library filename

        //Arrays to store our model's information
        std::vector<DWORD> indices;
        std::vector<XMFLOAT3> vertPos;
        std::vector<XMFLOAT3> vertNorm;
        std::vector<XMFLOAT2> vertTexCoord;
        std::vector<std::wstring> meshMaterials;

        //Vertex definition indices
        std::vector<int> vertPosIndex;
        std::vector<int> vertNormIndex;
        std::vector<int> vertTCIndex;

        //Make sure we have a default if no tex coords or normals are defined
        bool hasTexCoord = false;
        bool hasNorm = false;
        bool ComputeNormal = false;

        //Temp variables to store into vectors
        std::wstring meshMaterialsTemp;
        int vertPosIndexTemp;
        int vertNormIndexTemp;
        int vertTCIndexTemp;

        wchar_t checkChar;        //The variable we will use to store one char from file at a time
        std::wstring face;        //Holds the string containing our face vertices
        int vIndex = 0;            //Keep track of our vertex index count
        int triangleCount = 0;    //Total Triangles
        int totalVerts = 0;
        int meshTriangles = 0;

        //Check to see if the file was opened
        if (fileIn)
        {
            while (fileIn)
            {
                checkChar = fileIn.get();    //Get next char

                switch (checkChar)
                {
                case '#':
                    checkChar = fileIn.get();
                    while (checkChar != '\n')
                        checkChar = fileIn.get();
                    break;
                case 'v':    //Get Vertex Descriptions
                    checkChar = fileIn.get();
                    if (checkChar == ' ')    //v - vert position
                    {
                        float vz, vy, vx;
                        fileIn >> vx >> vy >> vz;    //Store the next three types

                        //if (isRHCoordSys)    //If model is from an RH Coord System
                        //    vertPos.push_back(XMFLOAT3(vx, vy, vz * -1.0f));    //Invert the Z axis
                        //else
                            vertPos.push_back(XMFLOAT3(vx, vy, vz));
                    }
                    if (checkChar == 't')    //vt - vert tex coords
                    {
                        float vtcu, vtcv;
                        fileIn >> vtcu >> vtcv;        //Store next two types

                        //if (isRHCoordSys)    //If model is from an RH Coord System
                        //    vertTexCoord.push_back(XMFLOAT2(vtcu, 1.0f - vtcv));    //Reverse the "v" axis
                        //else
                            vertTexCoord.push_back(XMFLOAT2(vtcu, vtcv));

                        hasTexCoord = true;    //We know the model uses texture coords
                    }
                    //Since we compute the normals later, we don't need to check for normals
                    //In the file, but i'll do it here anyway
                    if (checkChar == 'n')    //vn - vert normal
                    {
                        float vnx, vny, vnz;
                        fileIn >> vnx >> vny >> vnz;    //Store next three types

                        //if (isRHCoordSys)    //If model is from an RH Coord System
                        //    vertNorm.push_back(XMFLOAT3(vnx, vny, vnz * -1.0f));    //Invert the Z axis
                        //else
                            vertNorm.push_back(XMFLOAT3(vnx, vny, vnz));

                        hasNorm = true;    //We know the model defines normals
                    }
                    break;


                    //Get Face Index
                case 'f':    //f - defines the faces
                    checkChar = fileIn.get();
                    if (checkChar == ' ')
                    {
                        face = L"";
                        std::wstring VertDef;    //Holds one vertex definition at a time
                        triangleCount = 0;

                        checkChar = fileIn.get();
                        while (checkChar != '\n')
                        {
                            face += checkChar;            //Add the char to our face string
                            checkChar = fileIn.get();    //Get the next Character
                            if (checkChar == ' ')        //If its a space...
                                triangleCount++;        //Increase our triangle count
                        }

                        //Check for space at the end of our face string
                        if (face[face.length() - 1] == ' ')
                            triangleCount--;    //Each space adds to our triangle count

                        triangleCount -= 1;        //Ever vertex in the face AFTER the first two are new faces

                        std::wstringstream ss(face);

                        if (face.length() > 0)
                        {
                            int firstVIndex, lastVIndex;    //Holds the first and last vertice's index

                            for (int i = 0; i < 3; ++i)        //First three vertices (first triangle)
                            {
                                ss >> VertDef;    //Get vertex definition (vPos/vTexCoord/vNorm)

                                std::wstring vertPart;
                                int whichPart = 0;        //(vPos, vTexCoord, or vNorm)

                                //Parse this string
                                for (int j = 0; j < VertDef.length(); ++j)
                                {
                                    if (VertDef[j] != '/')    //If there is no divider "/", add a char to our vertPart
                                        vertPart += VertDef[j];

                                    //If the current char is a divider "/", or its the last character in the string
                                    if (VertDef[j] == '/' || j == VertDef.length() - 1)
                                    {
                                        std::wistringstream wstringToInt(vertPart);    //Used to convert wstring to int

                                        if (whichPart == 0)    //If vPos
                                        {
                                            wstringToInt >> vertPosIndexTemp;
                                            vertPosIndexTemp -= 1;        //subtract one since c++ arrays start with 0, and obj start with 1

                                            //Check to see if the vert pos was the only thing specified
                                            if (j == VertDef.length() - 1)
                                            {
                                                vertNormIndexTemp = 0;
                                                vertTCIndexTemp = 0;
                                            }
                                        }

                                        else if (whichPart == 1)    //If vTexCoord
                                        {
                                            if (vertPart != L"")    //Check to see if there even is a tex coord
                                            {
                                                wstringToInt >> vertTCIndexTemp;
                                                vertTCIndexTemp -= 1;    //subtract one since c++ arrays start with 0, and obj start with 1
                                            }
                                            else    //If there is no tex coord, make a default
                                                vertTCIndexTemp = 0;

                                            //If the cur. char is the second to last in the string, then
                                            //there must be no normal, so set a default normal
                                            if (j == VertDef.length() - 1)
                                                vertNormIndexTemp = 0;

                                        }
                                        else if (whichPart == 2)    //If vNorm
                                        {
                                            std::wistringstream wstringToInt(vertPart);

                                            wstringToInt >> vertNormIndexTemp;
                                            vertNormIndexTemp -= 1;        //subtract one since c++ arrays start with 0, and obj start with 1
                                        }

                                        vertPart = L"";    //Get ready for next vertex part
                                        whichPart++;    //Move on to next vertex part                    
                                    }
                                }


                                //Avoid duplicate vertices
                                bool vertAlreadyExists = false;
                                if (totalVerts >= 3)    //Make sure we at least have one triangle to check
                                {
                                    //Loop through all the vertices
                                    for (int iCheck = 0; iCheck < totalVerts; ++iCheck)
                                    {
                                        //If the vertex position and texture coordinate in memory are the same
                                        //As the vertex position and texture coordinate we just now got out
                                        //of the obj file, we will set this faces vertex index to the vertex's
                                        //index value in memory. This makes sure we don't create duplicate vertices
                                        if (vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
                                        {
                                            if (vertTCIndexTemp == vertTCIndex[iCheck])
                                            {
                                                indices.push_back(iCheck);        //Set index for this vertex
                                                vertAlreadyExists = true;        //If we've made it here, the vertex already exists
                                            }
                                        }
                                    }
                                }

                                //If this vertex is not already in our vertex arrays, put it there
                                if (!vertAlreadyExists)
                                {
                                    vertPosIndex.push_back(vertPosIndexTemp);
                                    vertTCIndex.push_back(vertTCIndexTemp);
                                    vertNormIndex.push_back(vertNormIndexTemp);
                                    totalVerts++;    //We created a new vertex
                                    indices.push_back(totalVerts - 1);    //Set index for this vertex
                                }

                                //If this is the very first vertex in the face, we need to
                                //make sure the rest of the triangles use this vertex
                                if (i == 0)
                                {
                                    firstVIndex = indices[vIndex];    //The first vertex index of this FACE

                                }

                                //If this was the last vertex in the first triangle, we will make sure
                                //the next triangle uses this one (eg. tri1(1,2,3) tri2(1,3,4) tri3(1,4,5))
                                if (i == 2)
                                {
                                    lastVIndex = indices[vIndex];    //The last vertex index of this TRIANGLE
                                }
                                vIndex++;    //Increment index count
                            }

                            meshTriangles++;    //One triangle down

                            //If there are more than three vertices in the face definition, we need to make sure
                            //we convert the face to triangles. We created our first triangle above, now we will
                            //create a new triangle for every new vertex in the face, using the very first vertex
                            //of the face, and the last vertex from the triangle before the current triangle
                            for (int l = 0; l < triangleCount - 1; ++l)    //Loop through the next vertices to create new triangles
                            {
                                //First vertex of this triangle (the very first vertex of the face too)
                                indices.push_back(firstVIndex);            //Set index for this vertex
                                vIndex++;

                                //Second Vertex of this triangle (the last vertex used in the tri before this one)
                                indices.push_back(lastVIndex);            //Set index for this vertex
                                vIndex++;

                                //Get the third vertex for this triangle
                                ss >> VertDef;

                                std::wstring vertPart;
                                int whichPart = 0;

                                //Parse this string (same as above)
                                for (int j = 0; j < VertDef.length(); ++j)
                                {
                                    if (VertDef[j] != '/')
                                        vertPart += VertDef[j];
                                    if (VertDef[j] == '/' || j == VertDef.length() - 1)
                                    {
                                        std::wistringstream wstringToInt(vertPart);

                                        if (whichPart == 0)
                                        {
                                            wstringToInt >> vertPosIndexTemp;
                                            vertPosIndexTemp -= 1;

                                            //Check to see if the vert pos was the only thing specified
                                            if (j == VertDef.length() - 1)
                                            {
                                                vertTCIndexTemp = 0;
                                                vertNormIndexTemp = 0;
                                            }
                                        }
                                        else if (whichPart == 1)
                                        {
                                            if (vertPart != L"")
                                            {
                                                wstringToInt >> vertTCIndexTemp;
                                                vertTCIndexTemp -= 1;
                                            }
                                            else
                                                vertTCIndexTemp = 0;
                                            if (j == VertDef.length() - 1)
                                                vertNormIndexTemp = 0;

                                        }
                                        else if (whichPart == 2)
                                        {
                                            std::wistringstream wstringToInt(vertPart);

                                            wstringToInt >> vertNormIndexTemp;
                                            vertNormIndexTemp -= 1;
                                        }

                                        vertPart = L"";
                                        whichPart++;
                                    }
                                }

                                //Check for duplicate vertices
                                bool vertAlreadyExists = false;
                                if (totalVerts >= 3)    //Make sure we at least have one triangle to check
                                {
                                    for (int iCheck = 0; iCheck < totalVerts; ++iCheck)
                                    {
                                        if (vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
                                        {
                                            if (vertTCIndexTemp == vertTCIndex[iCheck])
                                            {
                                                indices.push_back(iCheck);            //Set index for this vertex
                                                vertAlreadyExists = true;        //If we've made it here, the vertex already exists
                                            }
                                        }
                                    }
                                }

                                if (!vertAlreadyExists)
                                {
                                    vertPosIndex.push_back(vertPosIndexTemp);
                                    vertTCIndex.push_back(vertTCIndexTemp);
                                    vertNormIndex.push_back(vertNormIndexTemp);
                                    totalVerts++;                    //New vertex created, add to total verts
                                    indices.push_back(totalVerts - 1);        //Set index for this vertex
                                }

                                //Set the second vertex for the next triangle to the last vertex we got        
                                lastVIndex = indices[vIndex];    //The last vertex index of this TRIANGLE

                                meshTriangles++;    //New triangle defined
                                vIndex++;
                            }
                        }
                    }
                    break;


                default:
                    break;
                }
            }
        }
        else    //If we could not open the file
        {

            MessageBox(0, L"Models obj not found.", 0, 0);
            return;
        }


        //Make sure we have a default for the tex coord and normal
        //if one or both are not specified
        if (!hasNorm)
            vertNorm.push_back(XMFLOAT3(0.0f, 0.0f, 0.0f));
        if (!hasTexCoord)
            vertTexCoord.push_back(XMFLOAT2(0.0f, 0.0f));

        //Close the obj file, and open the mtl file
        fileIn.close();

        std::vector<Vertex> vertices;
        Vertex tempVert;

        //Create our vertices using the information we got 
        //from the file and store them in a vector
        for (int j = 0; j < totalVerts; ++j)
        {
            tempVert.Pos = vertPos[vertPosIndex[j]];
            tempVert.Normal = vertNorm[vertNormIndex[j]];
            tempVert.TexC = vertTexCoord[vertTCIndex[j]];

            vertices.push_back(tempVert);
        }

        //////////////////////Compute Normals///////////////////////////
        //If computeNormals was set to true then we will create our own
        //normals, if it was set to false we will use the obj files normals
        if (!hasNorm)
        {
            std::vector<XMFLOAT3> tempNormal;

            //normalized and unnormalized normals
            XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

            //Used to get vectors (sides) from the position of the verts
            float vecX, vecY, vecZ;

            //Two edges of our triangle
            XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
            XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

            //Compute face normals
            for (int i = 0; i < meshTriangles; ++i)
            {
                //Get the vector describing one edge of our triangle (edge 0,2)
                vecX = vertices[indices[(i * 3)]].Pos.x - vertices[indices[(i * 3) + 2]].Pos.x;
                vecY = vertices[indices[(i * 3)]].Pos.y - vertices[indices[(i * 3) + 2]].Pos.y;
                vecZ = vertices[indices[(i * 3)]].Pos.z - vertices[indices[(i * 3) + 2]].Pos.z;
                edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our first edge

                //Get the vector describing another edge of our triangle (edge 2,1)
                vecX = vertices[indices[(i * 3) + 2]].Pos.x - vertices[indices[(i * 3) + 1]].Pos.x;
                vecY = vertices[indices[(i * 3) + 2]].Pos.y - vertices[indices[(i * 3) + 1]].Pos.y;
                vecZ = vertices[indices[(i * 3) + 2]].Pos.z - vertices[indices[(i * 3) + 1]].Pos.z;
                edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our second edge

                //Cross multiply the two edge vectors to get the un-normalized face normal
                XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));
                tempNormal.push_back(unnormalized);            //Save unormalized normal (for normal averaging)
            }

            //Compute vertex normals (normal Averaging)
            XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
            int facesUsing = 0;
            float tX;
            float tY;
            float tZ;

            //Go through each vertex
            for (int i = 0; i < totalVerts; ++i)
            {
                //Check which triangles use this vertex
                for (int j = 0; j < meshTriangles; ++j)
                {
                    if (indices[j * 3] == i ||
                        indices[(j * 3) + 1] == i ||
                        indices[(j * 3) + 2] == i)
                    {
                        tX = XMVectorGetX(normalSum) + tempNormal[j].x;
                        tY = XMVectorGetY(normalSum) + tempNormal[j].y;
                        tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

                        normalSum = XMVectorSet(tX, tY, tZ, 0.0f);    //If a face is using the vertex, add the unormalized face normal to the normalSum
                        facesUsing++;
                    }
                }

                //Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
                normalSum = normalSum / facesUsing;

                //Normalize the normalSum vector
                normalSum = XMVector3Normalize(normalSum);

                //Store the normal in our current vertex
                vertices[i].Normal.x = XMVectorGetX(normalSum);
                vertices[i].Normal.y = XMVectorGetY(normalSum);
                vertices[i].Normal.z = XMVectorGetZ(normalSum);

                //Clear normalSum and facesUsing for next vertex
                normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
                facesUsing = 0;

            }
        }


        //
        // Pack the indices of all the meshes into one index buffer.
        //

        const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);

        const UINT ibByteSize = (UINT)indices.size() * sizeof(std::int32_t);

        auto geo = std::make_unique<MeshGeometry>();
        geo->Name = modelname + "Geo";

        ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
        CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

        ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
        CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

        geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
            mCommandList, vertices.data(), vbByteSize, geo->VertexBufferUploader);

        geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice,
            mCommandList, indices.data(), ibByteSize, geo->IndexBufferUploader);

        geo->VertexByteStride = sizeof(Vertex);
        geo->VertexBufferByteSize = vbByteSize;
        geo->IndexFormat = DXGI_FORMAT_R32_UINT;
        geo->IndexBufferByteSize = ibByteSize;

        SubmeshGeometry submesh;
        submesh.IndexCount = (UINT)indices.size();
        submesh.StartIndexLocation = 0;
        submesh.BaseVertexLocation = 0;
        //submesh.Bounds = bounds;

        geo->DrawArgs[modelname] = submesh;
        mSubmeshGeoList.push_back(std::make_pair(modelname, geo->Name));

        geometryMap[geo->Name] = std::move(geo);
        return;
}
