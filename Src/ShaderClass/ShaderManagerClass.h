#pragma once
#include <vector>
#include "Component/ComponentHeaders.h"
using namespace std;

class TextureShaderClass;
class BumpMapShaderClass;
class ProjectionShaderClass;
class GlowShaderClass;
class GlowMapShaderClass;
class HorizontalBlurShaderClass;
class VerticalBlurShaderClass;
class TransparentDepthShaderClass;
class ShadowDirectionalShaderClass;
class ParticleShaderClass;
class DeferredShaderClass;
class TextureInstanceShaderClass;
class TessellationShaderClass;
class DeferredMultiLightShaderClass;

class ShaderManagerClass
{
public:
	ShaderManagerClass();
	ShaderManagerClass(const ShaderManagerClass&);
	~ShaderManagerClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();

	bool RenderTextureShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

	bool RenderBumpMapShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);

	bool RenderProjectionShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*,
		XMFLOAT4, XMFLOAT4, XMFLOAT3, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

	bool RenderGlowShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float);

	bool RenderGlowMapShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*);

	bool RenderHorizontalBlurShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, float);

	bool RenderVerticalBlurShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, float);

	bool RenderTransparentDepthShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

	bool RenderShadowDirectionalShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4);

	bool RenderDeferredShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

	bool RenderParticleShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

	bool RenderTextureInstanceShader(ID3D11DeviceContext*, int, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

	bool RenderTessellationShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, float);

	bool RenderDeferredMultiLightShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*,  ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, vector<DirLight*>& dir, vector<PointLight*>& point, vector<SpotLight*>& spot);

	void InitializeLightBuffer(vector<DirLight*>& dir, vector<PointLight*>& point, vector<SpotLight*>& spot);

private:
	TextureShaderClass* m_TextureShader = nullptr;
	BumpMapShaderClass* m_BumpMapShader = nullptr;
	ProjectionShaderClass* m_ProjectionShader = nullptr;
	GlowShaderClass* m_GlowShader = nullptr;
	GlowMapShaderClass* m_GlowMapShader = nullptr;
	HorizontalBlurShaderClass* m_HorizontalBlurShader = nullptr;
	VerticalBlurShaderClass* m_VerticalBlurShader = nullptr;
	TransparentDepthShaderClass* m_TransparentDepthShader = nullptr;
	ShadowDirectionalShaderClass* m_ShadowDirectionalShader = nullptr;
	DeferredShaderClass* m_DeferredShader = nullptr;
	ParticleShaderClass* m_ParticleShader = nullptr;
	TextureInstanceShaderClass* m_InstanceTextureShader = nullptr;
	TessellationShaderClass* m_TessellationShader = nullptr;
	DeferredMultiLightShaderClass* m_DeferredMultiLightShader = nullptr;

};