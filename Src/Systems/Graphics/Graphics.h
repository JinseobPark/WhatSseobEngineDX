#pragma once
#include "Systems/System.h"
#include "Component/ComponentHeaders.h"
#include <vector>
#include <array>

using namespace std;

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.0f;
const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;
const float SHADOWMAP_DEPTH = 50.0f;
const float SHADOWMAP_NEAR = 1.0f;


class D3DClass;
//class CameraClass;
class ShaderManagerClass;
class TextureShaderClass;
class OrthoWindowClass;
class DeferredBuffersClass;


class Model;
class ModelInstance;
class ModelBillboard;
class ModelTessellation;
class PointLight;
class SpotLight;
class DirLight;

class Camera;

struct GraphicsDatas
{
	float f = 0.0f;
	int counter = 0;
	float clear_color[3] = { 0.45f, 0.55f, 0.60f };
	bool isResources = false;
	int resourceN = 1;
	bool show_another_window = true;
	float cubePosition[30][3] = { 0.0f, 0.0f, 0.0f };
	float cubeRotation[30][3] = { 0.0f, 0.0f, 0.0f };
	float cubePosition2[30][3] = { 0.0f, 0.0f, 0.0f };
	float billboardPosition[30][3] = { 0.0f, 0.0f, 0.0f };
	float tessellationAmount = 3.0f;
	bool isDeferred = true;

	float DirLightDirection[3][3] = { 0.0f, 0.0f, 0.0f };
	float PointLightPosition[30][3] = { 0.0f, 0.0f, 0.0f };
	float SpotLightPosition[30][3] = { 0.0f, 0.0f, 0.0f };
	float SpotLightDirection[30][3] = { 0.0f, 0.0f, 0.0f };

	int CameraNumber = 0;

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
	//Render
	void RenderTessellation();
	void RenderBillboard();
	void RenderModels();
	void RenderModelInstances();
	void RenderDeferredToTexture();
	void RenderDeferred();
	void RenderTexture();

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

	GraphicsDatas* g_data = nullptr;
};
extern Graphics* mGRAPHICS;