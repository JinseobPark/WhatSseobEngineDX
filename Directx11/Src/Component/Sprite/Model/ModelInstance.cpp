#include "stdafx.h"
#include "ModelInstance.h"
#include "../Texture.h"
#include "Systems/Graphics/D3DClass.h"
#include <fstream>
#include "Component/ComponentHeaders.h"
#include "Object/ObjectClass.h"
using namespace std;



ModelInstance::ModelInstance() : Component(CT_MODEL_INSTANCE)
{
	//Add model vector
	mGRAPHICS->m_modelInstances.push_back(this);
}

ModelInstance::~ModelInstance()
{
	//Delete model vector
	mGRAPHICS->m_modelInstances.erase(std::find(mGRAPHICS->m_modelInstances.begin(), mGRAPHICS->m_modelInstances.end(), this));
}


void ModelInstance::Initialize()
{
	// get transform pointer
	//this transform is parent's transform
	m_transform = (Transform*)GetOwner()->GetComponent(CT_TRANSFORM);
	// Add First object on original position. first child
	Transform* tr = new Transform();
	m_transforms.push_back(tr);

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


void ModelInstance::Shutdown()
{
	// release model textures
	ReleaseTextures();

	// Release vertex & index buffer
	ShutdownBuffers();

	// release model  data
	ReleaseModel();
}


void ModelInstance::Render(ID3D11DeviceContext* deviceContext)
{
	// set vertex & index buffer to pipeline for draw
	RenderBuffers(deviceContext);
}

int ModelInstance::GetVertexCount()
{
	return m_vertexCount;
}

int ModelInstance::GetInstanceCount()
{
	return m_instanceCount;
}

int ModelInstance::GetIndexCount()
{
	return m_indexCount;
}


ID3D11ShaderResourceView* ModelInstance::GetTexture(TextureType type)
{
	return m_textureManager->GetTexture(type);
}

/*****************************************************
ChangeInstanceBuffer : Update instance buffer. change children's information. NOT change childrens's size
*****************************************************/
bool ModelInstance::ChangeInstanceBuffer()
{
	HRESULT result;
	InstanceType* instancePtr;

	memset(m_instances, 0, (sizeof(InstanceType) * m_instanceCount));

	XMFLOAT3 originalPos = m_transform->getPosition();
	for (int i = 0; i < m_instanceCount; ++i)
	{
		XMFLOAT3 instancePos = m_transforms[i]->getPosition();
		m_instances[i].position = XMFLOAT3(originalPos.x + instancePos.x, originalPos.y + instancePos.y, originalPos.z + instancePos.z);
	}

	//get device context  from graphics directx
	ID3D11DeviceContext* deviceContext = mGRAPHICS->GetD3DClass()->GetDeviceContext();
	// Lock instance buffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = deviceContext->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// make pointer for instance data
	instancePtr = (InstanceType*)mappedResource.pData;

	//copy instance data to instance buffer
	memcpy(instancePtr, (void*)m_instances, (sizeof(InstanceType) * m_instanceCount));

	// unlock locked instance buffer
	deviceContext->Unmap(m_instanceBuffer, 0);


	return true;
}

/*****************************************************
ChangeInstanceNewBuffer : Update instance buffer. change children's information. Can change childrens's size
*****************************************************/
bool ModelInstance::ChangeInstanceNewBuffer()
{
	//release instance buffer
	m_instanceBuffer->Release();
	m_instanceBuffer = 0;

	//Make new instance buffer
	ID3D11Device* device = mGRAPHICS->GetD3DClass()->GetDevice();
	// set instance count
	m_instanceCount = m_transforms.size();

	// Allocate instance array  data
	m_instances = new InstanceType[m_instanceCount];
	memset(m_instances, 0, (sizeof(InstanceType) * m_instanceCount));

	// Set instance's transform / information
	XMFLOAT3 originalPos = m_transform->getPosition();
	for (int i = 0; i < m_instanceCount; ++i)
	{
		XMFLOAT3 instancePos = m_transforms[i]->getPosition();
		m_instances[i].position = XMFLOAT3(originalPos.x + instancePos.x, originalPos.y + instancePos.y, originalPos.z + instancePos.z);
	}

	// set dynamic instance buffer structures
	D3D11_BUFFER_DESC instanceBufferDesc;
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// make pointer for instance data
	D3D11_SUBRESOURCE_DATA instanceData;
	instanceData.pSysMem = m_instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// create instance buffer
	if (FAILED(device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer)))
	{
		return false;
	}
	return true;
}


bool ModelInstance::InitializeBuffers()
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
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

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

	// set instance count
	m_instanceCount = m_transforms.size();

	// Allocate instance array  data
	m_instances = new InstanceType[m_instanceCount];
	memset(m_instances, 0, (sizeof(InstanceType) * m_instanceCount));

	// Set instance's transform / information
	XMFLOAT3 originalPos = m_transform->getPosition();
	for (int i = 0; i < m_instanceCount; ++i)
	{
		XMFLOAT3 instancePos = m_transforms[i]->getPosition();
		m_instances[i].position = XMFLOAT3(originalPos.x + instancePos.x, originalPos.y + instancePos.y, originalPos.z + instancePos.z);
	}

	// set dynamic instance buffer structures
	D3D11_BUFFER_DESC instanceBufferDesc;
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// make pointer for instance data
	D3D11_SUBRESOURCE_DATA instanceData;
	instanceData.pSysMem = m_instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// create instance buffer
	if (FAILED(device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer)))
	{
		return false;
	}


	return true;
}


void ModelInstance::InitializeTextureManager()
{
	m_textureManager = new TextureManager();
}

void ModelInstance::ShutdownBuffers()
{
	// release vertex & index & instance  buffers
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if (m_instanceBuffer)
	{
		m_instanceBuffer->Release();
		m_instanceBuffer = 0;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}


void ModelInstance::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// Set offset and block of vertex buffer
	unsigned int strides[2] = { sizeof(VertexType), sizeof(InstanceType) };
	unsigned int offsets[2] = { 0, 0 };

	// set pointer array to vertex and instance buffer
	ID3D11Buffer* bufferPointers[2] = { m_vertexBuffer, m_instanceBuffer };

	// active vertex buffer to rendering
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// active index buffer to rendering
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Bind information about the primitive type
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void ModelInstance::AddTransform(Transform *transform)
{
	m_transforms.push_back(transform);
}

void ModelInstance::AddTextures(const WCHAR* filename, TextureType type)
{
	m_textureManager->AddTexture(mGRAPHICS->m_Direct3D->GetDevice(), filename, type);
}


void ModelInstance::ReleaseTextures()
{
	m_textureManager->ShutDown();
	delete m_textureManager;
	m_textureManager = 0;
}

bool ModelInstance::LoadModel(const char* filename)
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


void ModelInstance::ReleaseModel()
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
