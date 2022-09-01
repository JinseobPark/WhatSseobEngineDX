#include "stdafx.h"
#include "ShadowRenderTextures.h"


ShadowRenderTextures::ShadowRenderTextures()
{
}


ShadowRenderTextures::ShadowRenderTextures(const ShadowRenderTextures& other)
{
}


ShadowRenderTextures::~ShadowRenderTextures()
{
}


bool ShadowRenderTextures::Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear)
{
	HRESULT result;
	// Initialize the render target texture description.
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Set the render target texture description.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create render target
	for (int i = 0; i < MAX_DIRLIGHTS; i++)
	{
		result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture[i]);
		if (FAILED(result))
		{
			return false;
		}
	}


	// Set description render target view
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;


	// Create render target views
	for (int i = 0; i < MAX_DIRLIGHTS; i++)
	{
		result = device->CreateRenderTargetView(m_renderTargetTexture[i], &renderTargetViewDesc, &m_renderTargetView[i]);
		if (FAILED(result))
		{
			return false;
		}
	}


	// Set description of shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;


	// Create shader resource views
	for (int i = 0; i < MAX_DIRLIGHTS; i++)
	{
		result = device->CreateShaderResourceView(m_renderTargetTexture[i], &shaderResourceViewDesc, &m_shaderResourceView[i]);
		if (FAILED(result))
		{
			return false;
		}
	}

	// Initialize description of depth buffer
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set description of depth buffer
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create depth buffer
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Initialize depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set depth stencil view
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create depth stencil view
	result = device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	//Set viewport rendering
	m_viewport.Width = (float)textureWidth;
	m_viewport.Height = (float)textureHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	m_projectionMatrix = XMMatrixPerspectiveFovLH(((float)XM_PI / 4.0f), ((float)textureWidth / (float)textureHeight), screenNear, screenDepth);

	m_orthoMatrix = XMMatrixOrthographicLH((float)textureWidth, (float)textureHeight, screenNear, screenDepth);

	return true;
}


void ShadowRenderTextures::Shutdown()
{
	//Release
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	for (int i = 0; i < MAX_DIRLIGHTS; i++)
	{
		if (m_shaderResourceView[i])
		{
			m_shaderResourceView[i]->Release();
			m_shaderResourceView[i] = 0;
		}

		if (m_renderTargetView[i])
		{
			m_renderTargetView[i]->Release();
			m_renderTargetView[i] = 0;
		}

		if (m_renderTargetTexture[i])
		{
			m_renderTargetTexture[i]->Release();
			m_renderTargetTexture[i] = 0;
		}
	}
}


void ShadowRenderTextures::SetRenderTarget(ID3D11DeviceContext* deviceContext, int targetNum)
{
	//Bind render target view array and depth stencil buffer to render pipeline
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView[targetNum], m_depthStencilView);

	// Set viewport
	deviceContext->RSSetViewports(1, &m_viewport);
}


void ShadowRenderTextures::ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha, int targetNum)
{
	float color[4] = { red, green, blue, alpha };

	// Clear render target buffer
	deviceContext->ClearRenderTargetView(m_renderTargetView[targetNum], color);

	//Clear depth buffer
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}


ID3D11ShaderResourceView** ShadowRenderTextures::GetShaderResourceView()
{
	return m_shaderResourceView;
}

ID3D11ShaderResourceView* ShadowRenderTextures::GetShaderResourceView(int num)
{
	return m_shaderResourceView[num];
}



void ShadowRenderTextures::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}


void ShadowRenderTextures::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}