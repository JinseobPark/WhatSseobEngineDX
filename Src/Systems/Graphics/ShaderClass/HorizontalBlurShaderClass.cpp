#include "stdafx.h"
#include "HorizontalBlurShaderClass.h"


HorizontalBlurShaderClass::HorizontalBlurShaderClass()
{
}


HorizontalBlurShaderClass::HorizontalBlurShaderClass(const HorizontalBlurShaderClass& other)
{
}


HorizontalBlurShaderClass::~HorizontalBlurShaderClass()
{
}


bool HorizontalBlurShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	// Initialize vertex and pixel shader
	return InitializeShader(device, hwnd, L"data/hlsl/horizontalblurvs.hlsl", L"data/hlsl/horizontalblurps.hlsl");
}


void HorizontalBlurShaderClass::Shutdown()
{
	// Shutdown vertex and pixel shaders
	ShutdownShader();
}


bool HorizontalBlurShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float screenWidth)
{
	// Set shader parameters
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, screenWidth))
	{
		return false;
	}

	// Render the prepared buffers
	RenderShader(deviceContext, indexCount);

	return true;
}


bool HorizontalBlurShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;

	// Compile the vertex shader
	ID3D10Blob* vertexShaderBuffer = nullptr;
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "HorizontalBlurVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile, show message
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If nothing message, missed shader file
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader
	ID3D10Blob* pixelShaderBuffer = nullptr;
	result = D3DCompileFromFile(psFilename, NULL, NULL, "HorizontalBlurPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile, show message
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If nothing message, missed shader file
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create vertex shader from the buffer
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create pixel shader from the buffer
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of layout
	UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create vertex input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Release vertex and pixel shader buffer 
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Set description of the matrix dynamic constant buffer in the vertex shader.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있게 합니다.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	// Create matrix constant buffer pointer
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create matrix constant buffer pointer
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Create a texture sampler state description
	D3D11_BUFFER_DESC screenSizeBufferDesc;
	screenSizeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	screenSizeBufferDesc.ByteWidth = sizeof(ScreenSizeBufferType);
	screenSizeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	screenSizeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	screenSizeBufferDesc.MiscFlags = 0;
	screenSizeBufferDesc.StructureByteStride = 0;

	// Create the texture sampler state
	result = device->CreateBuffer(&screenSizeBufferDesc, NULL, &m_screenSizeBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void HorizontalBlurShaderClass::ShutdownShader()
{
	// Release buffers
	if (m_screenSizeBuffer)
	{
		m_screenSizeBuffer->Release();
		m_screenSizeBuffer = 0;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}


void HorizontalBlurShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	// Get a pointer to the error message text buffer.
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	// Return error message
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message 
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool HorizontalBlurShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float screenWidth)
{
	// Transpose the matrices for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Lock the matrix constant buffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// Get a pointer to the data
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy datas
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the matrix constant buffer
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the matrix constant buffer in the vertex shader
	unsigned int bufferNumber = 0;

	// Set the matrix constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Set shader texture array resource in the pixel shader.
	if (FAILED(deviceContext->Map(m_screenSizeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// Get a pointer to the data
	ScreenSizeBufferType* dataPtr2 = (ScreenSizeBufferType*)mappedResource.pData;

	// Copy datas
	dataPtr2->screenWidth = screenWidth;
	dataPtr2->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);;

	// Unlock the matrix constant buffer
	deviceContext->Unmap(m_screenSizeBuffer, 0);

	// Set the position of the matrix constant buffer in the vertex shader
	bufferNumber = 1;

	// Set the matrix constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_screenSizeBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}


void HorizontalBlurShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render
	deviceContext->DrawIndexed(indexCount, 0, 0);
}