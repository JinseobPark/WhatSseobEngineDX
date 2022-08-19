#include "stdafx.h"
#include "OrthoWindowClass.h"


OrthoWindowClass::OrthoWindowClass()
{
}


OrthoWindowClass::OrthoWindowClass(const OrthoWindowClass& other)
{
}


OrthoWindowClass::~OrthoWindowClass()
{
}


bool OrthoWindowClass::Initialize(ID3D11Device* device, int windowWidth, int windowHeight)
{
	// Initialize vertex and pixel shader
	return InitializeBuffers(device, windowWidth, windowHeight);
}


void OrthoWindowClass::Shutdown()
{
	// Shutdown vertex and pixel shaders
	ShutdownBuffers();
}


void OrthoWindowClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Render the prepared buffers
	RenderBuffers(deviceContext);
}


int OrthoWindowClass::GetIndexCount()
{
	return m_indexCount;
}


bool OrthoWindowClass::InitializeBuffers(ID3D11Device* device, int windowWidth, int windowHeight)
{
	float left = (float)((windowWidth / 2) * -1);
	float right = left + (float)windowWidth;
	float top = (float)(windowHeight / 2);
	float bottom = top - (float)windowHeight;

	m_vertexCount = 6;

	m_indexCount = m_vertexCount;

	// Create the vertex array
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array 
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Load data to vertex array
	// First triangle
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // 왼쪽 위
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // 오른쪽 아래
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // 왼쪽 아래
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// Second triangle
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // 왼쪽 위
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // 오른쪽 위
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // 오른쪽 아래
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// Load index array with data
	for (int i = 0; i < m_indexCount; i++)
	{
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

	// Release vertex and pixel buffer 
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void OrthoWindowClass::ShutdownBuffers()
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


void OrthoWindowClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// Set offset and block of vertex buffer
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// active vertex buffer to rendering
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// active index buffer to rendering
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Bind information about the primitive type
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}