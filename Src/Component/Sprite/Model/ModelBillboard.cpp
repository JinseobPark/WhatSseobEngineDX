#include "stdafx.h"
#include "ModelBillboard.h"
#include "../Texture.h"
#include "Systems/Graphics/D3DClass.h"
#include "Component/ComponentHeaders.h"
#include "Object/ObjectClass.h"
#include <fstream>
using namespace std;



ModelBillboard::ModelBillboard() : Component(CT_MODEL_BILLBOARD)
{
	//Add billboard model vector
	mGRAPHICS->m_modelBillboards.push_back(this);
}

ModelBillboard::~ModelBillboard()
{
	//Delete billboard model vector
	mGRAPHICS->m_modelBillboards.erase(std::find(mGRAPHICS->m_modelBillboards.begin(), mGRAPHICS->m_modelBillboards.end(), this));
}


void ModelBillboard::Initialize()
{
	// get transform pointer
	m_transform = (Transform*)GetOwner()->GetComponent(CT_TRANSFORM);

	// Initialize Vectex & Index buffer
	if (!InitializeBuffers())
	{
		assert("fail initialze buffer");
		return;
	}
	// Initalize Textuer manager
	InitializeTextureManager();
	return;
}


void ModelBillboard::Shutdown()
{
	// release model textures
	ReleaseTextures();

	// Release vertex & index buffer
	ShutdownBuffers();

	// release model  data
	ReleaseModel();
}


void ModelBillboard::Render(ID3D11DeviceContext* deviceContext)
{
	// set vertex & index buffer to pipeline for draw
	RenderBuffers(deviceContext);
}


int ModelBillboard::GetIndexCount()
{
	return m_indexCount;
}


ID3D11ShaderResourceView* ModelBillboard::GetTexture(TextureType type)
{
	return m_textureManager->GetTexture(type);
}


bool ModelBillboard::InitializeBuffers()
{
	//get device from graphics directx
	ID3D11Device* device = mGRAPHICS->GetD3DClass()->GetDevice();
	// Allocate vertex array data
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Allocate index array data
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Read vertex & index array from data
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);

		indices[i] = i;
	}

	// set static vertex buffer structures
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// make pointer for vertex data
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// create vertex buffer
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// set static index buffer structures
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// make pointer for index data
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// make pointer for index data
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// delete array
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void ModelBillboard::InitializeTextureManager()
{
	m_textureManager = new TextureManager();
}

void ModelBillboard::ShutdownBuffers()
{
	// release vertex & index buffers
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}


void ModelBillboard::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// Set offset and block of vertex buffer
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	// active vertex buffer to rendering
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// active index buffer to rendering
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Bind information about the primitive type
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void ModelBillboard::AddTextures(const WCHAR* filename, TextureType type)
{
	m_textureManager->AddTexture(mGRAPHICS->m_Direct3D->GetDevice(), filename, type);
}


void ModelBillboard::ReleaseTextures()
{
	m_textureManager->ShutDown();
	delete m_textureManager;
	m_textureManager = 0;
}

bool ModelBillboard::LoadModel(const char* filename)
{
	// Open model file
	ifstream fin;
	fin.open(filename);

	// if fail, exit
	if (fin.fail())
	{
		return false;
	}

	// skip read upto vertex count.
	char input = 0;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// read vertex count
	fin >> m_vertexCount;

	// set index count same as vertex count
	m_indexCount = m_vertexCount;

	// create model use read vertex count
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// skip read upto data starting point
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);
	
	// read vertex data
	for (int i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// close model file
	fin.close();

	return true;
}


void ModelBillboard::ReleaseModel()
{
	//release model , texture
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}
	if (m_textureManager)
	{
		delete[] m_textureManager;
		m_textureManager = 0;
	}
}
