#include "stdafx.h"
#include "TessellationShaderClass.h"


TessellationShaderClass::TessellationShaderClass()
{
}


TessellationShaderClass::TessellationShaderClass(const TessellationShaderClass& other)
{
}


TessellationShaderClass::~TessellationShaderClass()
{
}


bool TessellationShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	// 정점 및 픽셀 쉐이더를 초기화합니다.
	return InitializeShader(device, hwnd, L"data/hlsl/color_vs.hlsl", L"data/hlsl/color_hs.hlsl", L"data/hlsl/color_ds.hlsl", L"data/hlsl/color_ps.hlsl");
}


void TessellationShaderClass::Shutdown()
{
	// 버텍스 및 픽셀 쉐이더와 관련된 객체를 종료합니다.
	ShutdownShader();
}


bool TessellationShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float tessellationAmount)
{
	// 렌더링에 사용할 셰이더 매개 변수를 설정합니다.
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, tessellationAmount))
	{
		return false;
	}

	// 설정된 버퍼를 셰이더로 렌더링한다.
	RenderShader(deviceContext, indexCount);

	return true;
}


bool TessellationShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* hsFilename, const WCHAR* dsFilename, const WCHAR* psFilename)
{
	ID3D10Blob* errorMessage = nullptr;

	// 버텍스 쉐이더 코드를 컴파일한다.
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage)))
	{
		// 셰이더 컴파일 실패시 오류메시지를 출력합니다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 선체 쉐이더 코드를 컴파일합니다.
	ID3D10Blob* hullShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(hsFilename, NULL, NULL, "ColorHullShader", "hs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &hullShaderBuffer, &errorMessage)))
	{
		// 셰이더가 컴파일에 실패하면 오류 메시지에 무엇인가가 써 있어야합니다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, hsFilename);
		}
		// 오류 메시지에 아무 것도 없으면 단순히 셰이더 파일 자체를 찾을 수 없습니다.
		else
		{
			MessageBox(hwnd, hsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 도메인 쉐이더 코드를 컴파일한다.
	ID3D10Blob* domainShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(dsFilename, NULL, NULL, "ColorDomainShader", "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &domainShaderBuffer, &errorMessage)))
	{
		// 셰이더가 컴파일에 실패하면 오류 메시지에 무엇인가가 써 있어야합니다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, dsFilename);
		}
		// 오류 메시지에 아무 것도 없으면 단순히 셰이더 파일 자체를 찾을 수 없습니다.
		else
		{
			MessageBox(hwnd, dsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 픽셀 쉐이더 코드를 컴파일한다.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage)))
	{
		// 셰이더 컴파일 실패시 오류메시지를 출력합니다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 버퍼로부터 정점 셰이더를 생성한다.
	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader)))
	{
		return false;
	}

	// 버퍼에서 선체 쉐이더를 만듭니다.
	if (FAILED(device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), NULL, &m_hullShader)))
	{
		return false;
	}

	// 버퍼에서 도메인 셰이더를 만듭니다.
	if (FAILED(device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), NULL, &m_domainShader)))
	{
		return false;
	}

	// 버퍼에서 픽셀 쉐이더를 생성합니다.
	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader)))
	{
		return false;
	}

	// 정점 입력 레이아웃 구조체를 설정합니다.
	// 이 설정은 ModelClass와 셰이더의 VertexType 구조와 일치해야합니다.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// 레이아웃의 요소 수를 가져옵니다.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃을 만듭니다.
	if (FAILED(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))
	{
		return false;
	}

	// 더 이상 사용되지 않는 정점 셰이더 퍼버와 픽셀 셰이더 버퍼를 해제합니다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	hullShaderBuffer->Release();
	hullShaderBuffer = 0;

	domainShaderBuffer->Release();
	domainShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 정점 셰이더에 있는 행렬 상수 버퍼의 구조체를 작성합니다.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있게 합니다.
	if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer)))
	{
		return false;
	}

	// 선체 셰이더에있는 동적 테셀레이션 상수 버퍼의 설명을 설정합니다.
	D3D11_BUFFER_DESC tessellationBufferDesc;
	tessellationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessellationBufferDesc.ByteWidth = sizeof(TessellationBufferType);
	tessellationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDesc.MiscFlags = 0;
	tessellationBufferDesc.StructureByteStride = 0;

	// 이 클래스 내에서 선체 쉐이더 상수 버퍼에 액세스 할 수 있도록 상수 버퍼 포인터를 만듭니다.
	if (FAILED(device->CreateBuffer(&tessellationBufferDesc, NULL, &m_tessellationBuffer)))
	{
		return false;
	}

	return true;
}


void TessellationShaderClass::ShutdownShader()
{
	// 테셀레이션 상수 버퍼를 해제합니다.
	if (m_tessellationBuffer)
	{
		m_tessellationBuffer->Release();
		m_tessellationBuffer = 0;
	}

	// 행렬 상수 버퍼를 해제합니다.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// 레이아웃을 해제합니다.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// 픽셀 쉐이더를 해제합니다.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// 도메인 셰이더를 놓습니다.
	if (m_domainShader)
	{
		m_domainShader->Release();
		m_domainShader = 0;
	}

	// 선체 셰이더를 놓습니다.
	if (m_hullShader)
	{
		m_hullShader->Release();
		m_hullShader = 0;
	}

	// 버텍스 쉐이더를 놓습니다.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}


void TessellationShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	// 에러 메시지를 출력창에 표시합니다.
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	// 에러 메세지를 반환합니다.
	errorMessage->Release();
	errorMessage = 0;

	// 컴파일 에러가 있음을 팝업 메세지로 알려줍니다.
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}


bool TessellationShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, float tessellationAmount)
{
	// 행렬을 transpose하여 셰이더에서 사용할 수 있게 합니다
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 상수 버퍼에 행렬을 복사합니다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 상수 버퍼의 잠금을 풉니다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
	unsigned bufferNumber = 0;

	// 마지막으로 업데이트 된 값으로 도메인 셰이더의 행렬 상수 버퍼를 설정합니다.
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// 테설레이션 상수 버퍼를 쓸 수 있도록 잠금합니다.
	if (FAILED(deviceContext->Map(m_tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 테셀레이션 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	TessellationBufferType* dataPtr2 = (TessellationBufferType*)mappedResource.pData;

	// 테셀레이션 데이터를 상수 버퍼에 복사합니다.
	dataPtr2->tessellationAmount = tessellationAmount;
	dataPtr2->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// 테설레이션 상수 버퍼의 잠금을 해제합니다.
	deviceContext->Unmap(m_tessellationBuffer, 0);

	// 선체 셰이더에 테셀레이션 상수 버퍼의 위치를 ​​설정합니다.
	bufferNumber = 0;

	// 이제 업데이트 된 값으로 선체 쉐이더에서 테셀레이션 상수 버퍼를 설정합니다.
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_tessellationBuffer);

	return true;
}


void TessellationShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 정점 입력 레이아웃을 설정합니다.
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정합니다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->HSSetShader(m_hullShader, NULL, 0);
	deviceContext->DSSetShader(m_domainShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 삼각형을 그립니다.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}