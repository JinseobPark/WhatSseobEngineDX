#include "stdafx.h"
#include "BitmapClass_NoTexture.h"



BitmapClass_NoTexture::BitmapClass_NoTexture()
{
}


BitmapClass_NoTexture::BitmapClass_NoTexture(const BitmapClass_NoTexture& other)
{
}


BitmapClass_NoTexture::~BitmapClass_NoTexture()
{
}


bool BitmapClass_NoTexture::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight)
{
	// 화면 크기
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 렌더링 대상
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// 렌더링 위치
	m_previousPosX = -1;
	m_previousPosY = -1;

	// 버퍼 초기화
	if (!InitializeBuffers(device))
	{
		return false;
	}

	return true;
}


void BitmapClass_NoTexture::Shutdown()
{
	//버퍼 반환
	ShutdownBuffers();
}


bool BitmapClass_NoTexture::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	// 동적 위치 업데이트
	if (!UpdateBuffers(deviceContext, positionX, positionY))
	{
		return false;
	}

	//  버퍼 렌더링
	RenderBuffers(deviceContext);

	return true;
}


int BitmapClass_NoTexture::GetIndexCount()
{
	return m_indexCount;
}


bool BitmapClass_NoTexture::InitializeBuffers(ID3D11Device* device)
{
	// vertex 갯수 설정 사각형이므로 6개
	m_indexCount = m_vertexCount = 6;

	// create vertex array
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 배열 0으로 초기화
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// create index array
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// load index array 
	for (int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// vertex 버퍼구조 생성 설정
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// vertex buffer data pointer
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// create vertex buffer
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// index 버퍼구조 생성, 설정
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// index buffer data pointer
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// create index buffer
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// 생성되고 값이 할당된 정점 버퍼와 인덱스 버퍼를 해제합니다.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void BitmapClass_NoTexture::ShutdownBuffers()
{
	// index buffer
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// vertex buffer
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

bool BitmapClass_NoTexture::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;


	// 이 비트맵을 렌더링 할 위치가 변경되지 않은 경우 정점 버퍼를 업데이트 하지 마십시오.
	// 현재 올바른 매개 변수가 있습니다.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	// 변경된 경우 렌더링되는 위치를 업데이트합니다.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// 비트 맵 왼쪽의 화면 좌표를 계산합니다.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// 비트 맵 오른쪽의 화면 좌표를 계산합니다.
	right = left + (float)m_bitmapWidth;

	// 비트 맵 상단의 화면 좌표를 계산합니다.
	top = (float)(m_screenHeight / 2) - (float)positionY;

	// 비트 맵 아래쪽의 화면 좌표를 계산합니다.
	bottom = top - (float)m_bitmapHeight;

	// 정점 배열을 만듭니다.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 정점 배열에 데이터를로드합니다.
	// 첫 번째 삼각형
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// 두 번째 삼각형
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// 버텍스 버퍼를 쓸 수 있도록 잠급니다.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 정점 버퍼의 데이터를 가리키는 포인터를 얻는다.
	verticesPtr = (VertexType*)mappedResource.pData;

	// 데이터를 정점 버퍼에 복사합니다.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// 정점 버퍼의 잠금을 해제합니다.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// 더 이상 필요하지 않은 꼭지점 배열을 해제합니다.
	delete[] vertices;
	vertices = 0;

	return true;
}

void BitmapClass_NoTexture::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼의 단위와 오프셋을 설정합니다.
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형을 설정합니다. 여기서는 삼각형으로 설정합니다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

