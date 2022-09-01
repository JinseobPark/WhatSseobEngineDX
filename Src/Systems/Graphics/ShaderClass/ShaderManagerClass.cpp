#include "stdafx.h"
#include "shadermanagerclass.h"

#include "textureshaderclass.h"
#include "bumpmapshaderclass.h"
#include "ProjectionShaderClass.h"
#include "GlowMapShaderClass.h"
#include "GlowShaderClass.h"
#include "horizontalblurshaderclass.h"
#include "verticalblurshaderclass.h"
#include "transparentdepthshaderclass.h"
#include "ShadowDirectionalShaderClass.h"
#include "DeferredShaderClass.h"
#include "ParticleShaderClass.h"
#include "TextureInstanceShaderClass.h"
#include "TessellationShaderClass.h"
#include "DeferredMultiLightShaderClass.h"
#include "DepthShaderClass.h"
#include "DepthInstanceShaderClass.h"




ShaderManagerClass::ShaderManagerClass()
{
	m_TextureShader = 0;
	m_BumpMapShader = 0;
	m_ProjectionShader = 0;
	m_GlowMapShader = 0;
	m_GlowShader = 0;
	m_HorizontalBlurShader = 0;
	m_VerticalBlurShader = 0;
	m_TransparentDepthShader = 0;
	m_DeferredShader = 0;
	m_ParticleShader = 0;
	m_InstanceTextureShader = 0;
	m_TessellationShader = 0;
	m_DeferredMultiLightShader = 0;
	m_DepthShader = 0;
}


ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass& other)
{
}


ShaderManagerClass::~ShaderManagerClass()
{
}


bool ShaderManagerClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	// Create & Initialize All Shaders
	//Texture Shader
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}
	if (!m_TextureShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Bump Map Shader
	m_BumpMapShader = new BumpMapShaderClass;
	if (!m_BumpMapShader)
	{
		return false;
	}
	if (!m_BumpMapShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
		return false;
	}

	// Projection Shader
	m_ProjectionShader = new ProjectionShaderClass;
	if (!m_ProjectionShader)
	{
		return false;
	}
	if (!m_ProjectionShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
		return false;
	}

	// Glow Map Shader
	m_GlowMapShader = new GlowMapShaderClass;
	if (!m_GlowMapShader)
	{
		return false;
	}
	if (!m_GlowMapShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the glow map shader object.", L"Error", MB_OK);
		return false;
	}

	// Glow Shader
	m_GlowShader = new GlowShaderClass;
	if (!m_GlowShader)
	{
		return false;
	}
	if (!m_GlowShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the glow shader object.", L"Error", MB_OK);
		return false;
	}

	// Horizontal Blur Shader
	m_HorizontalBlurShader = new HorizontalBlurShaderClass;
	if (!m_HorizontalBlurShader)
	{
		return false;
	}
	if (!m_HorizontalBlurShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur shader object.", L"Error", MB_OK);
		return false;
	}

	// Vertical Blur Shader
	m_VerticalBlurShader = new VerticalBlurShaderClass;
	if (!m_VerticalBlurShader)
	{
		return false;
	}
	if (!m_VerticalBlurShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur shader object.", L"Error", MB_OK);
		return false;
	}

	// TransparentDepth Shader
	m_TransparentDepthShader = new TransparentDepthShaderClass;
	if (!m_TransparentDepthShader)
	{
		return false;
	}
	if (!m_TransparentDepthShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the transparent depth shader object.", L"Error", MB_OK);
		return false;
	}

	// Shadow Direction Shader
	m_ShadowDirectionalShader = new ShadowDirectionalShaderClass;
	if (!m_ShadowDirectionalShader)
	{
		return false;
	}
	if (!m_ShadowDirectionalShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
		return false;
	}

	//Deferred Shader
	m_DeferredShader = new DeferredShaderClass;
	if (!m_DeferredShader)
	{
		return false;
	}
	if (!m_DeferredShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the deferred shader object.", L"Error", MB_OK);
		return false;
	}


	// Particle Shader
	m_ParticleShader = new ParticleShaderClass;
	if (!m_ParticleShader)
	{
		return false;
	}
	if (!m_ParticleShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the particle shader object.", L"Error", MB_OK);
		return false;
	}

	// Instance Texture Shader
	m_InstanceTextureShader = new TextureInstanceShaderClass;
	if (!m_InstanceTextureShader)
	{
		return false;
	}
	if (!m_InstanceTextureShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	//Tessellation Shader
	m_TessellationShader = new TessellationShaderClass;
	if (!m_TessellationShader)
	{
		return false;
	}
	if (!m_TessellationShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the tessellation shader object.", L"Error", MB_OK);
		return false;
	}

	// Deferred Multi Light Shader
	m_DeferredMultiLightShader = new DeferredMultiLightShaderClass;
	if (!m_DeferredMultiLightShader)
	{
		return false;
	}
	if (!m_DeferredMultiLightShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Depth shader
	m_DepthShader = new DepthShaderClass;
	if (!m_DepthShader)
	{
		return false;
	}
	if (!m_DepthShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the Depth shader object.", L"Error", MB_OK);
		return false;
	}

	m_DepthInstanceShader = new DepthInstanceShaderClass;
	if (!m_DepthInstanceShader)
	{
		return false;
	}
	if (!m_DepthInstanceShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the Depth shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void ShaderManagerClass::Shutdown()
{
	// deallocate All Shader
	if (m_DepthInstanceShader)
	{
		m_DepthInstanceShader->Shutdown();
		delete m_DepthInstanceShader;
		m_DepthInstanceShader = 0;
	}

	if (m_DepthShader)
	{
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}

	if (m_DeferredMultiLightShader)
	{
		m_DeferredMultiLightShader->Shutdown();
		delete m_DeferredMultiLightShader;
		m_DeferredMultiLightShader = 0;
	}

	if (m_TessellationShader)
	{
		m_TessellationShader->Shutdown();
		delete m_TessellationShader;
		m_TessellationShader = 0;
	}

	if (m_InstanceTextureShader)
	{
		m_InstanceTextureShader->Shutdown();
		delete m_InstanceTextureShader;
		m_InstanceTextureShader = 0;
	}

	if (m_ParticleShader)
	{
		m_ParticleShader->Shutdown();
		delete m_ParticleShader;
		m_ParticleShader = 0;
	}

	if (m_DeferredShader)
	{
		m_DeferredShader->Shutdown();
		delete m_DeferredShader;
		m_DeferredShader = 0;
	}

	if (m_ShadowDirectionalShader)
	{
		m_ShadowDirectionalShader->Shutdown();
		delete m_ShadowDirectionalShader;
		m_ShadowDirectionalShader = 0;
	}

	if (m_TransparentDepthShader)
	{
		m_TransparentDepthShader->Shutdown();
		delete m_TransparentDepthShader;
		m_TransparentDepthShader = 0;
	}

	if (m_VerticalBlurShader)
	{
		m_VerticalBlurShader->Shutdown();
		delete m_VerticalBlurShader;
		m_VerticalBlurShader = 0;
	}

	if (m_HorizontalBlurShader)
	{
		m_HorizontalBlurShader->Shutdown();
		delete m_HorizontalBlurShader;
		m_HorizontalBlurShader = 0;
	}

	if (m_GlowShader)
	{
		m_GlowShader->Shutdown();
		delete m_GlowShader;
		m_GlowShader = 0;
	}

	if (m_GlowMapShader)
	{
		m_GlowMapShader->Shutdown();
		delete m_GlowMapShader;
		m_GlowMapShader = 0;
	}

	if (m_ProjectionShader)
	{
		m_ProjectionShader->Shutdown();
		delete m_ProjectionShader;
		m_ProjectionShader = 0;
	}

	if (m_BumpMapShader)
	{
		m_BumpMapShader->Shutdown();
		delete m_BumpMapShader;
		m_BumpMapShader = 0;
	}

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}
}


bool ShaderManagerClass::RenderTextureShader(ID3D11DeviceContext* device, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture)
{
	return m_TextureShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}


bool ShaderManagerClass::RenderBumpMapShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture, XMFLOAT3 lightDirection,
	XMFLOAT4 diffuse)
{
	return m_BumpMapShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, colorTexture, normalTexture, lightDirection, diffuse);
}

bool ShaderManagerClass::RenderProjectionShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor,
	XMFLOAT3 lightDirection, XMMATRIX viewMatrix2, XMMATRIX projectionMatrix2,
	ID3D11ShaderResourceView* projectionTexture)
{
	return m_ProjectionShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, ambientColor, diffuseColor, lightDirection, viewMatrix2, projectionMatrix2, projectionTexture);
}

bool ShaderManagerClass::RenderGlowShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* glowTexture,
	float glowStrength)
{
	return m_GlowShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, colorTexture, glowTexture, glowStrength);
}

bool ShaderManagerClass::RenderGlowMapShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* glowMap)
{
	return m_GlowMapShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, glowMap);
}

bool ShaderManagerClass::RenderHorizontalBlurShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float screenWidth)
{
	return m_HorizontalBlurShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, screenWidth);
}

bool ShaderManagerClass::RenderVerticalBlurShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float screenHeight)
{
	return m_VerticalBlurShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, screenHeight);
}

bool ShaderManagerClass::RenderTransparentDepthShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	return m_TransparentDepthShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}

bool ShaderManagerClass::RenderShadowDirectionalShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix,
	ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, XMFLOAT3 lightDirection,
	XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor)
{
	return m_ShadowDirectionalShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, texture, depthMapTexture, lightDirection, ambientColor, diffuseColor);
}

bool ShaderManagerClass::RenderDeferredShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	return m_DeferredShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}

bool ShaderManagerClass::RenderParticleShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	return m_ParticleShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}

bool ShaderManagerClass::RenderTextureInstanceShader(ID3D11DeviceContext* deviceContext, int vertexCount, int instanceCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	return m_InstanceTextureShader->Render(deviceContext, vertexCount, instanceCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}

bool ShaderManagerClass::RenderTessellationShader(ID3D11DeviceContext* deviceContext, int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float tessellationAmount)
{
	return m_TessellationShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, tessellationAmount);
}

bool ShaderManagerClass::RenderDeferredMultiLightShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* posTexture, ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture, ID3D11ShaderResourceView* depthTexture,
			vector<DirLight*>& dir, vector<PointLight*>& point, vector<SpotLight*>& spot, ID3D11ShaderResourceView** shadow)
{
	return m_DeferredMultiLightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, posTexture, colorTexture, normalTexture, depthTexture, dir, point, spot, shadow);
}

bool ShaderManagerClass::RenderDepthShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	return m_DepthShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix);
}

bool ShaderManagerClass::RenderDepthInstanceShader(ID3D11DeviceContext* deviceContext, int indexCount, int instanceCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	return m_DepthInstanceShader->Render(deviceContext, indexCount, instanceCount, worldMatrix, viewMatrix, projectionMatrix);
}

// Initialize Light Buffer. This active in initialize step
void ShaderManagerClass::InitializeLightBuffer(vector<DirLight*> &dir, vector<PointLight*> &point , vector<SpotLight*> &spot)
{
	m_DeferredMultiLightShader->InitializeLightBuffer(dir, point, spot);
}

