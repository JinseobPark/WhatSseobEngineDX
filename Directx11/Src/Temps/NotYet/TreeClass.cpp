#include "stdafx.h"
#include "Component/Sprite/Texture.h"
#include "treeclass.h"

#include <fstream>
using namespace std;


TreeClass::TreeClass()
{
}


TreeClass::TreeClass(const TreeClass& other)
{
}


TreeClass::~TreeClass()
{
}


bool TreeClass::Initialize(ID3D11Device* device, const char* trunkModelFilename, const WCHAR* trunkTextureFilename,
	const char* leafModelFilename, const WCHAR* leafTextureFilename, float scale)
{
	// 나무 모델 트렁크 데이터를 로드합니다.
	bool result = LoadModel(trunkModelFilename);
	if (!result)
	{
		return false;
	}

	// 트렁크 인덱스 수를 저장하십시오.
	m_trunkIndexCount = m_indexCount;

	// 트리 트렁크의 기하 구조를 보유하고 있는 정점과 인덱스 버퍼를 초기화합니다.
	result = InitializeTrunkBuffers(device, scale);
	if (!result)
	{
		return false;
	}

	// 버퍼에 로드된 트리 트렁크 모델 데이터를 해제합니다.
	ReleaseModel();

	// 트리 구조 모델 데이터를 로드합니다.
	result = LoadModel(leafModelFilename);
	if (!result)
	{
		return false;
	}

	// 리프 인덱스 수를 저장합니다.
	m_leafIndexCount = m_indexCount;

	// 트리 나뭇잎에 대한 지오 메트릭을 포함하는 정점 및 인덱스 버퍼를 초기화합니다.
	result = InitializeLeafBuffers(device, scale);
	if (!result)
	{
		return false;
	}

	// 트리 구조 모델 데이터를 해제합니다.
	ReleaseModel();

	// 트리 모델의 텍스처를 로드합니다.
	result = LoadTextures(device, trunkTextureFilename, leafTextureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}


void TreeClass::Shutdown()
{
	// 텍스처를 해제한다.
	ReleaseTextures();

	// 버텍스와 인덱스 버퍼를 해제한다.
	ShutdownBuffers();

	// 아직 릴리즈 되지 않은 모델 데이터를 해제합니다.
	ReleaseModel();
}


void TreeClass::RenderTrunk(ID3D11DeviceContext* deviceContext)
{
	// 그리기를 준비하기 위해 그래픽 파이프 라인에 꼭지점과 인덱스 버퍼를 놓습니다.
	RenderTrunkBuffers(deviceContext);
}


void TreeClass::RenderLeaves(ID3D11DeviceContext* deviceContext)
{
	// 그리기를 준비하기 위해 그래픽 파이프 라인에 꼭지점과 인덱스 버퍼를 놓습니다.
	RenderLeafBuffers(deviceContext);
}


int TreeClass::GetTrunkIndexCount()
{
	return m_trunkIndexCount;
}


int TreeClass::GetLeafIndexCount()
{
	return m_leafIndexCount;
}


ID3D11ShaderResourceView* TreeClass::GetTrunkTexture()
{
	return m_TrunkTexture->GetTexture();
}


ID3D11ShaderResourceView* TreeClass::GetLeafTexture()
{
	return m_LeafTexture->GetTexture();
}


bool TreeClass::InitializeTrunkBuffers(ID3D11Device* device, float scale)
{
	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// 정점 배열과 인덱스 배열을 데이터로 로드합니다.
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x * scale, m_model[i].y * scale, m_model[i].z * scale);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	// 정적 정점 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_trunkVertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_trunkIndexBuffer)))
	{
		return false;
	}

	// 이제 버텍스와 인덱스 버퍼가 생성되고 로드된 배열을 해제합니다.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


bool TreeClass::InitializeLeafBuffers(ID3D11Device* device, float scale)
{
	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// 정점 배열과 인덱스 배열을 데이터로 로드합니다.
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x * scale, m_model[i].y * scale, m_model[i].z * scale);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	// 정적 정점 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_leafVertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_leafIndexBuffer)))
	{
		return false;
	}

	// 이제 버텍스와 인덱스 버퍼가 생성되고 로드된 배열을 해제합니다.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void TreeClass::ShutdownBuffers()
{
	// 버퍼를 해제합니다.
	if (m_leafIndexBuffer)
	{
		m_leafIndexBuffer->Release();
		m_leafIndexBuffer = 0;
	}

	if (m_leafVertexBuffer)
	{
		m_leafVertexBuffer->Release();
		m_leafVertexBuffer = 0;
	}

	if (m_trunkIndexBuffer)
	{
		m_trunkIndexBuffer->Release();
		m_trunkIndexBuffer = 0;
	}

	if (m_trunkVertexBuffer)
	{
		m_trunkVertexBuffer->Release();
		m_trunkVertexBuffer = 0;
	}
}


void TreeClass::RenderTrunkBuffers(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	deviceContext->IASetVertexBuffers(0, 1, &m_trunkVertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	deviceContext->IASetIndexBuffer(m_trunkIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//이 꼭지점 버퍼에서 렌더링되어야 하는 프리미티브 유형을 설정합니다.이 경우에는 삼각형입니다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void TreeClass::RenderLeafBuffers(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	deviceContext->IASetVertexBuffers(0, 1, &m_leafVertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	deviceContext->IASetIndexBuffer(m_leafIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//이 꼭지점 버퍼에서 렌더링되어야하는 프리미티브 유형을 설정합니다.이 경우에는 삼각형입니다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool TreeClass::LoadTextures(ID3D11Device* device, const WCHAR* trunkFilename, const WCHAR* leafFilename)
{
	// 트렁크 텍스처 객체를 만듭니다.
	m_TrunkTexture = new Texture;
	if (!m_TrunkTexture)
	{
		return false;
	}

	// 트렁크 텍스처 객체를 초기화 합니다.
	if (!m_TrunkTexture->Initialize(device, trunkFilename))
	{
		return false;
	}

	// 리프 텍스처 객체를 만듭니다.
	m_LeafTexture = new Texture;
	if (!m_LeafTexture)
	{
		return false;
	}

	// 리프 텍스처 객체를 초기화 합니다.
	if (!m_LeafTexture->Initialize(device, leafFilename))
	{
		return false;
	}

	return true;
}


void TreeClass::ReleaseTextures()
{
	// 텍스처 오브젝트를 릴리즈한다.
	if (m_LeafTexture)
	{
		m_LeafTexture->Shutdown();
		delete m_LeafTexture;
		m_LeafTexture = 0;
	}

	if (m_TrunkTexture)
	{
		m_TrunkTexture->Shutdown();
		delete m_TrunkTexture;
		m_TrunkTexture = 0;
	}
}


bool TreeClass::LoadModel(const char* filename)
{
	// 모델 파일을 엽니다.
	ifstream fin;
	fin.open(filename);

	// 파일을 열수 없으면 종료합니다.
	if (fin.fail())
	{
		return false;
	}

	// 버텍스 카운트의 값까지 읽는다.
	char input = 0;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// 버텍스 카운트를 읽는다.
	fin >> m_vertexCount;

	// 인덱스의 수를 정점수와 같게 설정합니다.
	m_indexCount = m_vertexCount;

	// 읽어 들인 정점 개수를 사용하여 모델을 만듭니다.
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// 데이터의 시작 부분까지 읽는다.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// 버텍스 데이터를 읽습니다.
	for (int i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// 모델 파일을 닫는다.
	fin.close();

	return true;
}


void TreeClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}
}


void TreeClass::SetPosition(XMFLOAT3 pos)
{
	m_position = pos;
}

void TreeClass::GetPosition(XMFLOAT3& pos)
{
	pos = m_position;
}