#include "stdafx.h"
#include "TextureClass.h"
#include "ModelInstanceClass.h"

#include <fstream>
using namespace std;

ModelInstanceClass::ModelInstanceClass()
{
}


ModelInstanceClass::ModelInstanceClass(const ModelInstanceClass& other)
{
}


ModelInstanceClass::~ModelInstanceClass()
{
}


bool ModelInstanceClass::Initialize(ID3D11Device* device, const WCHAR* textureFilename, char* modelFilename)
{
	bool result;
	// �� �����͸� �ε��մϴ�.
	result = (LoadModel(modelFilename));
	if (!result)
	{
		return false;
	}

	result = InitializeBuffers(device);
	// ���� �� �ε��� ���۸� �ʱ�ȭ�մϴ�.
	if (!result)
	{
		return false;
	}

	// �� ���� �ؽ�ó�� �ε��մϴ�.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}


void ModelInstanceClass::Shutdown()
{
	// �� �ؽ��ĸ� ��ȯ�մϴ�.
	ReleaseTexture();

	// ���ؽ� �� �ε��� ���۸� �����մϴ�.
	ShutdownBuffers();

	// �� ������ ��ȯ
	ReleaseModel();
}


void ModelInstanceClass::Render(ID3D11DeviceContext* deviceContext)
{
	// �׸��⸦ �غ��ϱ� ���� �׷��� ������ ���ο� �������� �ε��� ���۸� �����ϴ�.
	RenderBuffers(deviceContext);
}


int ModelInstanceClass::GetVertexCount()
{
	return m_vertexCount;
}


int ModelInstanceClass::GetInstanceCount()
{
	return m_instanceCount;
}

int ModelInstanceClass::GetIndexCount()
{
	return m_indexCount;
}


ID3D11ShaderResourceView* ModelInstanceClass::GetTexture()
{
	return m_Texture->GetTexture();
}


bool ModelInstanceClass::InitializeBuffers(ID3D11Device* device)
{
	// ���� �迭�� ���� ���� �����մϴ�.
	//m_vertexCount = 3;

	// ���� �迭�� ����ϴ�.
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// �ε��� �迭�� ����ϴ�.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// ���� �迭�� �ε��� �迭�� �����ͷ� �о�ɴϴ�.
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	//// ���� �迭�� ���� �����մϴ�.
	//vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	//vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

	//vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
	//vertices[1].texture = XMFLOAT2(0.5f, 0.0f);

	//vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	//vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

	// ���� ���� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ���۸� ����ϴ�.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// ���� �ε��� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ��մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� �����մϴ�.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}


	// ���� ���ؽ� ���۰� �����ǰ�ε� �� ���ؽ� �迭�� �����Ѵ�.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	// �迭�� �ν��Ͻ� ���� �����մϴ�.
	m_instanceCount = 10;

	// �ν��Ͻ� �迭�� ����ϴ�.
	InstanceType* instances = new InstanceType[m_instanceCount];
	if (!instances)
	{
		return false;
	}

	// �����ͷ� �ν��Ͻ� �迭�� �ε��մϴ�.
	//instances[0].position = XMFLOAT3(-1.5f, -1.5f, 5.0f);
	//instances[1].position = XMFLOAT3(-1.5f, 1.5f, 5.0f);
	//instances[2].position = XMFLOAT3(1.5f, -1.5f, 5.0f);
	//instances[3].position = XMFLOAT3(1.5f, 1.5f, 5.0f);
	for (int i = 0; i < m_instanceCount; ++i)
	{
		instances[i].position = XMFLOAT3(-10.0f + i, 1.5f, 5.0f);
	}

	// �ν��Ͻ� ������ ������ �����մϴ�.
	D3D11_BUFFER_DESC instanceBufferDesc;
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// ���� ���ҽ� ������ �ν��Ͻ� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA instanceData;
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// �ν��Ͻ� ���۸� ����ϴ�.
	if (FAILED(device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer)))
	{
		return false;
	}

	// �ν��Ͻ� ���۰� �����ǰ�ε�Ǿ����Ƿ� �ν��Ͻ� �迭�� �����մϴ�.
	delete[] instances;
	instances = 0;

	return true;
}


void ModelInstanceClass::ShutdownBuffers()
{
	// �ε��� ���۸� �����մϴ�.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// �ν��Ͻ� ���۸� �����մϴ�.
	if (m_instanceBuffer)
	{
		m_instanceBuffer->Release();
		m_instanceBuffer = 0;
	}

	// ���� ���۸� �����մϴ�.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}


void ModelInstanceClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// ���� �����°� ��Ʈ���̵带 �����մϴ�.
	unsigned int strides[2] = { sizeof(VertexType), sizeof(InstanceType) };
	unsigned int offsets[2] = { 0, 0 };

	// �������� �迭�� ���� ���ۿ� �ν��Ͻ� ���۷� �����մϴ�.
	ID3D11Buffer* bufferPointers[2] = { m_vertexBuffer, m_instanceBuffer };

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �� ������ ���ۿ��� �������Ǿ���ϴ� ������Ƽ�� ������ �����մϴ�.�� ��쿡�� �ﰢ���Դϴ�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool ModelInstanceClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	// �ؽ�ó ������Ʈ�� �����Ѵ�.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// �ؽ�ó ������Ʈ�� �ʱ�ȭ�Ѵ�.
	return m_Texture->Initialize(device, filename);
}


void ModelInstanceClass::ReleaseTexture()
{
	// �ؽ�ó ������Ʈ�� �������Ѵ�.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}

bool ModelInstanceClass::LoadModel(char* filename)
{
	// �� ������ ���ϴ�.
	ifstream fin;
	fin.open(filename);

	// ������ �� �� ������ �����մϴ�.
	if (fin.fail())
	{
		return false;
	}

	// ���ؽ� ī��Ʈ�� ������ �д´�.
	char input = 0;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// ���ؽ� ī��Ʈ�� �д´�.
	fin >> m_vertexCount;

	// �ε����� ���� ���� ���� ���� �����մϴ�.
	m_indexCount = m_vertexCount;

	// �о� ���� ���� ������ ����Ͽ� ���� ����ϴ�.
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// �������� ���� �κб��� �д´�.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// ���ؽ� �����͸� �н��ϴ�.
	for (int i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// �� ������ �ݴ´�.
	fin.close();

	return true;
}


void ModelInstanceClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}
}

bool ModelInstanceClass::ChangeInstanceBuffer(ID3D11DeviceContext* deviceContext, float movement)
{
	HRESULT result;
	InstanceType* instancePtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	// �迭�� �ν��Ͻ� ���� �����մϴ�.
	m_instanceCount = 5;

	// �ν��Ͻ� �迭�� ����ϴ�.
	InstanceType* instances = new InstanceType[m_instanceCount];
	if (!instances) return false;

	for (int i = 0; i < m_instanceCount; ++i)
	{
		instances[i].position = XMFLOAT3(3 * i, movement, 5.0f);
	}

	result = deviceContext->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ������ �����͸� ����Ű�� �����͸� ��´�.
	instancePtr = (InstanceType*)mappedResource.pData;

	// �����͸� ���� ���ۿ� �����մϴ�.
	memcpy(instancePtr, (void*)instances, (sizeof(InstanceType) * m_instanceCount));

	// ���� ������ ����� �����մϴ�.
	deviceContext->Unmap(m_instanceBuffer, 0);

	// �ν��Ͻ� ���۰� �����ǰ�ε�Ǿ����Ƿ� �ν��Ͻ� �迭�� �����մϴ�.
	delete[] instances;
	instances = 0;

	return true;
}
