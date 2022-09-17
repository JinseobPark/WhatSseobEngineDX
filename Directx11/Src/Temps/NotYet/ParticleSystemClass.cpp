#include "stdafx.h"
#include "Component/Sprite/Texture.h"
#include "particlesystemclass.h"


ParticleSystemClass::ParticleSystemClass()
{
}


ParticleSystemClass::ParticleSystemClass(const ParticleSystemClass& other)
{
}


ParticleSystemClass::~ParticleSystemClass()
{
}


bool ParticleSystemClass::Initialize(ID3D11Device* device, const WCHAR* textureFilename)
{
	// 파티클에 사용되는 텍스처를 로드합니다.
	if (!LoadTexture(device, textureFilename))
	{
		return false;
	}

	// 파티클 시스템을 초기화합니다.
	if (!InitializeParticleSystem())
	{
		return false;
	}

	// 파티클을 렌더링하는 데 사용할 버퍼를 만듭니다.
	if (!InitializeBuffers(device))
	{
		return false;
	}

	return true;
}


void ParticleSystemClass::Shutdown()
{
	// 버퍼를 해제합니다.
	ShutdownBuffers();

	// 파티클 시스템을 해제합니다.
	ShutdownParticleSystem();

	// 파티클에 사용된 텍스처를 해제합니다.
	ReleaseTexture();
}


bool ParticleSystemClass::Frame(float frameTime, ID3D11DeviceContext* deviceContext)
{
	// 오래된 파티클을 해제합니다.
	KillParticles();

	// 새 파티클을 방출합니다.
	EmitParticles(frameTime);

	// 파티클 위치를 업데이트 합니다.
	UpdateParticles(frameTime);

	// 동적 정점 버퍼를 각 파티클의 새 위치로 업데이트합니다.
	return UpdateBuffers(deviceContext);
}


void ParticleSystemClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 리기를 준비하기 위해 그래픽 파이프 라인에 꼭지점과 인덱스 버퍼를 놓습니다.
	RenderBuffers(deviceContext);
}


ID3D11ShaderResourceView* ParticleSystemClass::GetTexture()
{
	return m_Texture->GetTexture();
}


int ParticleSystemClass::GetIndexCount()
{
	return m_indexCount;
}


bool ParticleSystemClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	// 텍스처 오브젝트를 생성한다.
	m_Texture = new Texture;
	if (!m_Texture)
	{
		return false;
	}

	// 텍스처 오브젝트를 초기화한다.
	return m_Texture->Initialize(device, filename);
}


void ParticleSystemClass::ReleaseTexture()
{
	// 텍스처 오브젝트를 릴리즈한다.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}


bool ParticleSystemClass::InitializeParticleSystem()
{
	// 방출 될 때 파티클이 위치 할 수 있는 임의의 편차를 설정합니다.
	m_particleDeviationX = 0.5f;
	m_particleDeviationY = 0.1f;
	m_particleDeviationZ = 2.0f;

	// 파티클의 속도와 속도 변화를 설정합니다.
	m_particleVelocity = 1.0f;
	m_particleVelocityVariation = 0.2f;

	// 파티클의 물리적 크기를 설정합니다.
	m_particleSize = 0.2f;

	// 초당 방출 할 파티클 수를 설정합니다.
	m_particlesPerSecond = 250.0f;

	// 파티클 시스템에 허용되는 최대 파티클 수를 설정합니다.
	m_maxParticles = 5000;

	// 파티클 리스트를 생성합니다.
	m_particleList = new ParticleType[m_maxParticles];
	if (!m_particleList)
	{
		return false;
	}

	// 파티클 리스트를 초기화 합니다.
	for (int i = 0; i < m_maxParticles; i++)
	{
		m_particleList[i].active = false;
	}

	// 아직 배출되지 않으므로 현재 파티클 수를 0으로 초기화합니다.
	m_currentParticleCount = 0;

	// 초당 파티클 방출 속도의 초기 누적 시간을 지웁니다.
	m_accumulatedTime = 0.0f;

	return true;
}


void ParticleSystemClass::ShutdownParticleSystem()
{
	// 파티클 목록을 해제합니다.
	if (m_particleList)
	{
		delete[] m_particleList;
		m_particleList = 0;
	}
}


bool ParticleSystemClass::InitializeBuffers(ID3D11Device* device)
{
	// 정점 배열의 최대 정점 수를 설정합니다.
	m_vertexCount = m_maxParticles * 6;

	// 인덱스 배열의 최대 인덱스 수를 설정합니다.
	m_indexCount = m_vertexCount;

	// 렌더링 될 입자에 대한 정점 배열을 만듭니다.
	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices)
	{
		return false;
	}

	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// 처음에는 정점 배열을 0으로 초기화합니다.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 인덱스 배열을 초기화합니다.
	for (int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// 동적 정점 버퍼의 설명을 설정한다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 마침내 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 설명을 설정합니다.
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
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// 더 이상 필요하지 않으므로 인덱스 배열을 해제합니다.
	delete[] indices;
	indices = 0;

	return true;
}


void ParticleSystemClass::ShutdownBuffers()
{
	// 인덱스 버퍼를 해제합니다.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// 버텍스 버퍼를 해제한다.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}


void ParticleSystemClass::EmitParticles(float frameTime)
{
	// 프레임 시간을 증가시킵니다.
	m_accumulatedTime += frameTime;

	// 이제 파티클 출력을 false로 설정합니다.
	bool emitParticle = false;

	// 새 파티클을 방출할 시간인지 확인합니다.
	if (m_accumulatedTime > (1000.0f / m_particlesPerSecond))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// 방출 할 파티클이 있으면 프레임 당 하나씩 방출합니다.
	if ((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1)))
	{
		m_currentParticleCount++;

		// 이제 임의 화 된 파티클 속성을 생성합니다.
		float positionX = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationX;
		float positionY = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationY;
		float positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationZ;

		float velocity = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;

		float red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		float green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		float blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		// 이제 블렌딩을 위해 파티클을 뒤에서 앞으로 렌더링해야하므로 파티클 배열을 정렬해야 합니다.
		// Z 깊이를 사용하여 정렬하므로 목록에서 파티클을 삽입해야 하는 위치를 찾아야합니다.
		int index = 0;
		bool found = false;
		while (!found)
		{
			if ((m_particleList[index].active == false) || (m_particleList[index].positionZ < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		// 삽입 할 위치를 알았으므로 인덱스에서 한 위치 씩 배열을 복사하여 새 파티클을 위한 공간을 만들어야합니다.
		int i = m_currentParticleCount;
		int j = i - 1;

		while (i != index)
		{
			m_particleList[i].positionX = m_particleList[j].positionX;
			m_particleList[i].positionY = m_particleList[j].positionY;
			m_particleList[i].positionZ = m_particleList[j].positionZ;
			m_particleList[i].red = m_particleList[j].red;
			m_particleList[i].green = m_particleList[j].green;
			m_particleList[i].blue = m_particleList[j].blue;
			m_particleList[i].velocity = m_particleList[j].velocity;
			m_particleList[i].active = m_particleList[j].active;
			i--;
			j--;
		}

		// 이제 정확한 깊이 순서로 파티클 배열에 삽입하십시오.
		m_particleList[index].positionX = positionX;
		m_particleList[index].positionY = positionY;
		m_particleList[index].positionZ = positionZ;
		m_particleList[index].red = red;
		m_particleList[index].green = green;
		m_particleList[index].blue = blue;
		m_particleList[index].velocity = velocity;
		m_particleList[index].active = true;
	}
}


void ParticleSystemClass::UpdateParticles(float frameTime)
{
	// 각 프레임은 위치, 속도 및 프레임 시간을 사용하여 아래쪽으로 이동하여 모든 파티클을 업데이트합니다.
	for (int i = 0; i < m_currentParticleCount; i++)
	{
		m_particleList[i].positionY = m_particleList[i].positionY - (m_particleList[i].velocity * frameTime * 0.001f);
	}
}


void ParticleSystemClass::KillParticles()
{
	// 특정 높이 범위를 벗어난 모든 파티클을 제거합니다.
	for (int i = 0; i < m_maxParticles; i++)
	{
		if ((m_particleList[i].active == true) && (m_particleList[i].positionY < -3.0f))
		{
			m_particleList[i].active = false;
			m_currentParticleCount--;

			// 이제 모든 살아있는 파티클을 배열위로 이동시켜 파괴된 파티클을 지우고 배열을 올바르게 정렬된 상태로 유지합니다.
			for (int j = i; j < m_maxParticles - 1; j++)
			{
				m_particleList[j].positionX = m_particleList[j + 1].positionX;
				m_particleList[j].positionY = m_particleList[j + 1].positionY;
				m_particleList[j].positionZ = m_particleList[j + 1].positionZ;
				m_particleList[j].red = m_particleList[j + 1].red;
				m_particleList[j].green = m_particleList[j + 1].green;
				m_particleList[j].blue = m_particleList[j + 1].blue;
				m_particleList[j].velocity = m_particleList[j + 1].velocity;
				m_particleList[j].active = m_particleList[j + 1].active;
			}
		}
	}
}


bool ParticleSystemClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	// 처음에는 정점 배열을 0으로 초기화합니다.
	VertexType* verticesPtr;
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 이제 파티클 목록 배열에서 정점 배열을 만듭니다. 각 파티클은 두 개의 삼각형으로 만들어진 쿼드입니다.
	int index = 0;

	for (int i = 0; i < m_currentParticleCount; i++)
	{
		// 왼쪽 아래.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 왼쪽 위.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 오른쪽 아래.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 오른쪽 아래.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 왼쪽 위.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// 오른쪽 위.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;
	}

	// 정점 버퍼를 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 정점 버퍼의 데이터를 가리키는 포인터를 얻는다.
	verticesPtr = (VertexType*)mappedResource.pData;

	// 데이터를 정점 버퍼에 복사합니다.
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

	// 정점 버퍼의 잠금을 해제합니다.
	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}


void ParticleSystemClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 정점 버퍼에서 렌더링 되어야 하는 프리미티브 유형을 설정합니다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}