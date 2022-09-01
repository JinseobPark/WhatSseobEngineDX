#pragma once
#include "Systems/System.h"
#include "Component/ComponentHeaders.h"
#include "Systems/GlobalDefines.h"
#include <vector>
#include <array>

using namespace std;



class D3DClass;
//class CameraClass;
class ShaderManagerClass;
class TextureShaderClass;
class OrthoWindowClass;
class DeferredBuffersClass;
class RenderTexture;
class ShadowRenderTextures;

class Model;
class ModelInstance;
class ModelBillboard;
class ModelTessellation;
class PointLight;
class SpotLight;
class DirLight;

class Camera;

enum class RenderList
{
	NORMAL = 0,
	DEFERRED,
	RESOURCES,
	SHADOWS
};

struct GraphicsDatas
{
	float f = 0.0f;
	int counter = 0;
	float clear_color[3] = { 0.45f, 0.55f, 0.60f };
	int renderlist = 1;
	int resourceN = 1;
	int resourceShadow = 1;
	bool show_another_window = true;
	float cubePosition[30][3] = { 0.0f, 0.0f, 0.0f };
	float cubeRotation[30][3] = { 0.0f, 0.0f, 0.0f };
	float cubePosition2[30][3] = { 0.0f, 0.0f, 0.0f };
	float billboardPosition[30][3] = { 0.0f, 0.0f, 0.0f };
	float tessellationAmount = 3.0f;
	bool isDeferred = true;

	float DirLightDirection[MAX_DIRLIGHTS][3] = { 0.0f, 0.0f, 0.0f };
	float PointLightPosition[MAX_POINTLIGHTS][3] = { 0.0f, 0.0f, 0.0f };
	float SpotLightPosition[MAX_SPOTLIGHTS][3] = { 0.0f, 0.0f, 0.0f };
	float SpotLightDirection[MAX_SPOTLIGHTS][3] = { 0.0f, 0.0f, 0.0f };

	int CameraNumber = 0;

};

enum class RenderType
{
	NONE = 0,
	SHADOW,
	DEFERRED,
	FORWARD
};


class Graphics : public System
{
public:
	Graphics();
	~Graphics();

	void Initialize() override;
	void Update(float dt) override;
	void Shutdown();

	void InitializeLightInfo();
	D3DClass* GetD3DClass();
private:

	//Shadow
	void MakeShadow();

	//Deferred
	void RenderTessellation(RenderType render);
	void RenderBillboard(RenderType render);
	void RenderModels(RenderType render);
	void RenderModelInstances(RenderType render);

	//Render List
	void RenderDeferredToTexture();
	void RenderDeferred();
	void RenderResources();
	void RenderShadows();

	//forward
	void RenderNormal();



	//Imgui
	void ImguiInitialize();
	void ImguiLoadInfo();
	void ImguiUpdate(float dt);
	void ImguiShutdown();


public:
	D3DClass* m_Direct3D = nullptr;
	std::vector<Model*> m_models;
	std::vector<ModelInstance*> m_modelInstances;
	std::vector<ModelBillboard*> m_modelBillboards;
	std::vector<ModelTessellation*> m_modelTessellations;
	std::vector<DirLight*> m_dirLights;
	std::vector<PointLight*> m_pointLights;
	std::vector<SpotLight*> m_spotLights;

	std::vector<Camera*> m_Camera;
	unsigned int m_CameraNumber;

private:
	ShaderManagerClass* m_ShaderManager = nullptr;
	OrthoWindowClass* m_FullScreenWindow = nullptr;
	DeferredBuffersClass* m_DeferredBuffers = nullptr;
	ShadowRenderTextures* m_ShadowTexture = nullptr;

	//std::vector<pair<RenderTexture*, DirLight*>> m_ShadowTextures;
	RenderList m_renderlist = RenderList::DEFERRED;

	GraphicsDatas* g_data = nullptr;
	unsigned int m_shadowNum;
};
extern Graphics* mGRAPHICS;