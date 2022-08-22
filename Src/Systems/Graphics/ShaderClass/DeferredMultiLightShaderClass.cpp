#include "stdafx.h"
#include "DeferredMultiLightShaderClass.h"
#include "Systems/Graphics/D3DClass.h"


DeferredMultiLightShaderClass::DeferredMultiLightShaderClass()
{
	m_dirLightCount = 0;
	m_pointLightCount = 0;
	m_spotLightCount = 0;
}


DeferredMultiLightShaderClass::~DeferredMultiLightShaderClass()
{
}


bool DeferredMultiLightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	// Initialize vertex and pixel shaders
	return InitializeShader(device, hwnd, L"data/hlsl/deferredmultilight_vs.hlsl", L"data/hlsl/deferredmultilight_ps.hlsl");
}


void DeferredMultiLightShaderClass::Shutdown()
{
	// Shutdown vertex and pixel shaders
	ShutdownShader();
}


bool DeferredMultiLightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* posTexture, ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture, ID3D11ShaderResourceView* depthTexture, 
	vector<DirLight*>& dir, vector<PointLight*>& point, vector<SpotLight*>& spot)
{
	// Set shader parameters
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, posTexture, colorTexture, normalTexture, depthTexture, dir, point, spot))
	{
		return false;
	}

	// Render the prepared buffers
	RenderShader(deviceContext, indexCount);

	return true;
}

bool DeferredMultiLightShaderClass::InitializeLightBuffer(vector<DirLight*> &dir, vector<PointLight*> &point, vector<SpotLight*> &spot)
{
	ID3D11Device* device = mGRAPHICS->GetD3DClass()->GetDevice();
	HRESULT result;

	LightCountBufferType *m_lightCount = new LightCountBufferType;
	memset(m_lightCount, 0, sizeof(LightCountBufferType));

	m_lightCount->dirLightCount = dir.size();
	m_lightCount->PointLightCount = point.size();
	m_lightCount->SpotLightCount = spot.size();
	m_lightCount->padding = 0.0f;
	
	// Set description of the light dynamic constant buffer in the pixel shader.
	D3D11_BUFFER_DESC lightCountBufferDesc;
	lightCountBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightCountBufferDesc.ByteWidth = sizeof(LightCountBufferType);
	lightCountBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightCountBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightCountBufferDesc.MiscFlags = 0;
	lightCountBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA LightCountData;
	LightCountData.pSysMem = m_lightCount;
	LightCountData.SysMemPitch = 0;
	LightCountData.SysMemSlicePitch = 0;

	// Create the constant buffer pointer 
	result = device->CreateBuffer(&lightCountBufferDesc, &LightCountData, &m_lightCountBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release buffer
	delete m_lightCount;
	m_lightCount = 0;

	// Directional Light 
	if (dir.size() > 0)
	{
		m_dirLightCount = dir.size();

		// Create Directional light  array
		m_dirLight = new DirLightBufferType[m_dirLightCount];
		memset(m_dirLight, 0, (sizeof(DirLightBufferType) * m_dirLightCount));

		for (int i = 0; i < m_dirLightCount; ++i)
		{
			m_dirLight[i].m_position = dir.at(i)->m_position;
			m_dirLight[i].m_direction = dir.at(i)->m_direction;
			m_dirLight[i].m_ambient = dir.at(i)->m_ambient;
			m_dirLight[i].m_diffuse = dir.at(i)->m_diffuse;
		}

		D3D11_BUFFER_DESC DirlightBufferDesc;
		DirlightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		DirlightBufferDesc.ByteWidth = sizeof(DirLightBufferType) * m_dirLightCount;
		DirlightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		DirlightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		DirlightBufferDesc.MiscFlags = 0;
		DirlightBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA dirLightData;
		dirLightData.pSysMem = m_dirLight;
		dirLightData.SysMemPitch = 0;
		dirLightData.SysMemSlicePitch = 0;

		result = device->CreateBuffer(&DirlightBufferDesc, &dirLightData, &m_dirlightBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}

	//Point Light
	if (point.size() > 0)
	{
		m_pointLightCount = point.size();

		// Create point light  array
		m_pointLight = new PointLightBufferType[m_pointLightCount];
		memset(m_pointLight, 0, (sizeof(PointLightBufferType) * m_pointLightCount));

		for (int i = 0; i < m_pointLightCount; ++i)
		{
			m_pointLight[i].m_position = point.at(i)->m_position;
			m_pointLight[i].padding = 0.0f;
			m_pointLight[i].m_diffuse = point.at(i)->m_diffuse;
			m_pointLight[i].m_ambient = point.at(i)->m_ambient;
			m_pointLight[i].m_constant = point.at(i)->m_constant;
			m_pointLight[i].m_linear = point.at(i)->m_linear;
			m_pointLight[i].m_quadratic = point.at(i)->m_quadratic;
			m_pointLight[i].padding2 = 0.0f;
		}

		D3D11_BUFFER_DESC PointlightBufferDesc;
		PointlightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		PointlightBufferDesc.ByteWidth = sizeof(PointLightBufferType) * m_pointLightCount;
		PointlightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		PointlightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		PointlightBufferDesc.MiscFlags = 0;
		PointlightBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA pointLightData;
		pointLightData.pSysMem = m_pointLight;
		pointLightData.SysMemPitch = 0;
		pointLightData.SysMemSlicePitch = 0;

		result = device->CreateBuffer(&PointlightBufferDesc, &pointLightData, &m_pointlightBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}

	if (spot.size() > 0)
	{
		m_spotLightCount = spot.size();
		// Create spot light  array
		m_spotLight = new SpotLightBufferType[m_spotLightCount];
		memset(m_spotLight, 0, (sizeof(SpotLightBufferType) * m_spotLightCount));

		for (int i = 0; i < m_spotLightCount; ++i)
		{
			m_spotLight[i].m_position = spot.at(i)->m_position;
			m_spotLight[i].m_cutOff = spot.at(i)->m_cutOff;
			m_spotLight[i].m_direction = spot.at(i)->m_direction;
			m_spotLight[i].m_outerCutOff = spot.at(i)->m_outerCutOff;
			m_spotLight[i].m_diffuse = spot.at(i)->m_diffuse;
			m_spotLight[i].m_ambient = spot.at(i)->m_ambient;
			m_spotLight[i].m_constant = spot.at(i)->m_constant;
			m_spotLight[i].m_linear = spot.at(i)->m_linear;
			m_spotLight[i].m_quadratic = spot.at(i)->m_quadratic;
			m_spotLight[i].padding = 0.0f;
		}

		D3D11_BUFFER_DESC SpotlightBufferDesc;
		SpotlightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		SpotlightBufferDesc.ByteWidth = sizeof(SpotLightBufferType) * m_spotLightCount;
		SpotlightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		SpotlightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		SpotlightBufferDesc.MiscFlags = 0;
		SpotlightBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA spotLightData;
		spotLightData.pSysMem = m_spotLight;
		spotLightData.SysMemPitch = 0;
		spotLightData.SysMemSlicePitch = 0;

		// 이 클래스 내에서 정점 셰이더 상수 버퍼에 액세스 할 수 있도록 상수 버퍼 포인터를 만듭니다.
		result = device->CreateBuffer(&SpotlightBufferDesc, &spotLightData, &m_spotlightBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}

}


bool DeferredMultiLightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;

	// Compile the vertex shader
	ID3D10Blob* vertexShaderBuffer = nullptr;
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
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
	result = D3DCompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
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

	// Create a texture sampler state description
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	// Set description of the matrix dynamic constant buffer in the vertex shader.
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

	return true;
}


void DeferredMultiLightShaderClass::ShutdownShader()
{	
	// Release buffers

	if (m_dirLight)
	{
		m_dirLight = nullptr;
	}
	if (m_pointLight)
	{
		m_pointLight = nullptr;
	}
	if (m_spotLight)
	{
		m_spotLight = nullptr;
	}


	if (m_dirlightBuffer)
	{
		m_dirlightBuffer->Release();
		m_dirlightBuffer = 0;
	}
	if (m_pointlightBuffer)
	{
		m_pointlightBuffer->Release();
		m_pointlightBuffer = 0;
	}
	if (m_spotlightBuffer)
	{
		m_spotlightBuffer->Release();
		m_spotlightBuffer = 0;
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


void DeferredMultiLightShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	// Get a pointer to the error message text buffer.
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	// Return error message
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message 
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool DeferredMultiLightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* posTexture, ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture, ID3D11ShaderResourceView* depthTexture, 
	vector<DirLight*>& dir, vector<PointLight*>& point, vector<SpotLight*>& spot)
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

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &posTexture);
	deviceContext->PSSetShaderResources(1, 1, &colorTexture);
	deviceContext->PSSetShaderResources(2, 1, &normalTexture);
	deviceContext->PSSetShaderResources(3, 1, &depthTexture);

	// Lock the light constant buffer
	if (FAILED(deviceContext->Map(m_lightCountBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// Get a pointer to the data
	LightCountBufferType* dataPtr2 = (LightCountBufferType*)mappedResource.pData;

	// Copy datas
	dataPtr2->dirLightCount = dir.size();
	dataPtr2->PointLightCount = point.size();
	dataPtr2->SpotLightCount = spot.size();
	dataPtr2->padding = 0.0f;

	// Unlock the constant buffer
	deviceContext->Unmap(m_lightCountBuffer, 0);

	// Set the position constant buffer
	bufferNumber = 0;

	// Set the constant buffer in the pixel shader
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightCountBuffer);

	bufferNumber++;

	/*********************************************************
		DirLightBuffer
	********************************************************/
	if (dir.size())
	{
		if (FAILED(deviceContext->Map(m_dirlightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			return false;
		}
		DirLightBufferType* dataPtr3 = (DirLightBufferType*)mappedResource.pData;

		for (int i = 0; i < m_dirLightCount; ++i)
		{
			dataPtr3[i].m_position = dir.at(i)->m_position;
			dataPtr3[i].m_direction = dir.at(i)->m_direction;
			dataPtr3[i].m_ambient = dir.at(i)->m_ambient;
			dataPtr3[i].m_diffuse = dir.at(i)->m_diffuse;
		}
		deviceContext->Unmap(m_dirlightBuffer, 0);

		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_dirlightBuffer);
	}

	bufferNumber++;

	/*********************************************************
		PointLightBuffer
	********************************************************/
	if (point.size())
	{
		if (FAILED(deviceContext->Map(m_pointlightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			return false;
		}
		PointLightBufferType* dataPtr4 = (PointLightBufferType*)mappedResource.pData;

		for (int i = 0; i < m_pointLightCount; ++i)
		{
			dataPtr4[i].m_position = point.at(i)->m_position;
			dataPtr4[i].padding = 0.0f;
			dataPtr4[i].m_diffuse = point.at(i)->m_diffuse;
			dataPtr4[i].m_ambient = point.at(i)->m_ambient;
			dataPtr4[i].m_constant = point.at(i)->m_constant;
			dataPtr4[i].m_linear = point.at(i)->m_linear;
			dataPtr4[i].m_quadratic = point.at(i)->m_quadratic;
			dataPtr4[i].padding2 = 0.0f;
		}
		deviceContext->Unmap(m_pointlightBuffer, 0);
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pointlightBuffer);
	}

	bufferNumber++;

	/*********************************************************
		SpotLightBuffer
	********************************************************/
	if (spot.size())
	{
		if (FAILED(deviceContext->Map(m_spotlightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			return false;
		}
		SpotLightBufferType* dataPtr5 = (SpotLightBufferType*)mappedResource.pData;

		for (int i = 0; i < m_spotLightCount; ++i)
		{
			dataPtr5[i].m_position = spot.at(i)->m_position;
			dataPtr5[i].m_cutOff = spot.at(i)->m_cutOff;
			dataPtr5[i].m_direction = spot.at(i)->m_direction;
			dataPtr5[i].m_outerCutOff = spot.at(i)->m_outerCutOff;
			dataPtr5[i].m_diffuse = spot.at(i)->m_diffuse;
			dataPtr5[i].m_ambient = spot.at(i)->m_ambient;
			dataPtr5[i].m_constant = spot.at(i)->m_constant;
			dataPtr5[i].m_linear = spot.at(i)->m_linear;
			dataPtr5[i].m_quadratic = spot.at(i)->m_quadratic;
			dataPtr5[i].padding = 0.0f;
		}
		deviceContext->Unmap(m_spotlightBuffer, 0);
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_spotlightBuffer);
	}


	return true;
}


void DeferredMultiLightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangles
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

void DeferredMultiLightShaderClass::UpdateLightInfo()
{

}
