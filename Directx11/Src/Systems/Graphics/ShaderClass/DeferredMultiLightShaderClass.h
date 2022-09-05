#pragma once
#include "Component/ComponentHeaders.h"
#include <vector>
using namespace std;

class DirLight;
class PointLight;
class SpotLight;


class DeferredMultiLightShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightCountBufferType
	{
		int dirLightCount;
		int PointLightCount;
		int SpotLightCount;
		float padding;
	};

	struct DirLightBufferType
	{
		XMFLOAT3 m_position;
		float padding1;
		XMFLOAT3 m_direction;
		float padding2;
		XMFLOAT4 m_ambient;
		XMFLOAT4 m_diffuse;
		XMMATRIX m_lightVP;
	};

	struct PointLightBufferType
	{
		XMFLOAT3 m_position;
		float padding;
		XMFLOAT4 m_diffuse;
		XMFLOAT4 m_ambient;
		float m_constant;
		float m_linear;
		float m_quadratic;
		float padding2;
	};

	struct SpotLightBufferType
	{
		XMFLOAT3 m_position;
		float m_cutOff;
		XMFLOAT3 m_direction;
		float m_outerCutOff;
		XMFLOAT4 m_diffuse;
		XMFLOAT4 m_ambient;
		float m_constant;
		float m_linear;
		float m_quadratic;
		float padding;
	};
public:
	DeferredMultiLightShaderClass();
	DeferredMultiLightShaderClass(const DeferredMultiLightShaderClass&);
	~DeferredMultiLightShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, vector<DirLight*>& dir, vector<PointLight*>& point, vector<SpotLight*>& spot, ID3D11ShaderResourceView** shadow);
	//bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, std::vector<DirLight>);

	bool InitializeLightBuffer(vector<DirLight*>& dir, vector<PointLight*>& point, vector<SpotLight*>& spot);

private:
	bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, vector<DirLight*>& dir, vector<PointLight*>& point, vector<SpotLight*>& spot, ID3D11ShaderResourceView** shadow);
	void RenderShader(ID3D11DeviceContext*, int);

	void UpdateLightInfo();
private:
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
	//ID3D11SamplerState* m_sampleState = nullptr;
	ID3D11SamplerState* m_sampleStateWrap = nullptr;
	ID3D11SamplerState* m_sampleStateClamp = nullptr;
	ID3D11SamplerState* m_sampleStateShadow = nullptr;
	ID3D11Buffer* m_matrixBuffer = nullptr;

	ID3D11Buffer* m_lightCountBuffer = nullptr;
	ID3D11Buffer* m_dirlightBuffer = nullptr;
	ID3D11Buffer* m_pointlightBuffer = nullptr;
	ID3D11Buffer* m_spotlightBuffer = nullptr;

	int m_dirLightCount;
	int m_pointLightCount;
	int m_spotLightCount;
	DirLightBufferType* m_dirLight = nullptr;
	PointLightBufferType* m_pointLight = nullptr;
	SpotLightBufferType* m_spotLight = nullptr;

};