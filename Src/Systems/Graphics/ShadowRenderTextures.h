#pragma once
#include "Systems/GlobalDefines.h"


class ShadowRenderTextures : public AlignedAllocationPolicy<16>
{
public:
	ShadowRenderTextures();
	ShadowRenderTextures(const ShadowRenderTextures&);
	~ShadowRenderTextures();

	bool Initialize(ID3D11Device*, int, int, float, float);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*, int);
	void ClearRenderTarget(ID3D11DeviceContext*, float, float, float, float, int);

	ID3D11ShaderResourceView** GetShaderResourceView();
	ID3D11ShaderResourceView* GetShaderResourceView(int num);

	void GetProjectionMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

private:
	ID3D11Texture2D* m_renderTargetTexture[MAX_DIRLIGHTS] = { 0 };
	ID3D11RenderTargetView* m_renderTargetView[MAX_DIRLIGHTS] = { 0 };
	ID3D11ShaderResourceView* m_shaderResourceView[MAX_DIRLIGHTS] = { 0 };
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	D3D11_VIEWPORT m_viewport;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_orthoMatrix;
};