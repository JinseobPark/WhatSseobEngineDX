#pragma once
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

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////

class D3DClass;
class CameraClass;
class ModelClass;
class ModelClass2;
class LightClass;
class PointLightClass;
class LightShaderClass;
class MultiLightShaderClass;
class RenderTextureClass;
class DebugWindowClass;
class SpecMapShaderClass;
class TextureShaderClass;
class FogShaderClass;
class ClipPlaneShaderClass;
class TranslateShaderClass;
class TransparentShaderClass;
class ReflectionShaderClass;
class BitmapClass_NoTexture;
class FadeShaderClass;
class RefractionShaderClass;
class WaterShaderClass;
class GlassShaderClass;
class ModelNormalClass;
class FireShaderClass;
class ModelBillboardClass;
class DepthShaderClass;
class HorizontalBlurShaderClass;
class VerticalBlurShaderClass;
class OrthoWindowClass;
class RenderBlurTextureClass;
class ModelInstanceClass;
class TextureInstanceShaderClass;
class ModelTeselClass;
class ColorShaderClass;
class ParticleShaderClass;
class ParticleSystemClass;
class ShadowShaderClass;
class ShadowMultiShaderClass;
class SoftShadowShaderClass;
class ShadowBlackWhiteShaderClass;

class TextureClass;
class ProjectionShaderClass;
class ViewPointClass;

class ShaderManagerClass;
class BitmapGlowClass;
class TreeClass;

class DeferredBuffersClass;


struct GraphicsDatas
{
	float f = 0.0f;
	int counter = 0;
	float clear_color[3] = { 0.45f, 0.55f, 0.60f };
	bool Deferred = true;
	bool show_another_window = true;
	float firePosition[3] = { 0, 0, 0 };
};

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float, XMFLOAT3& position);
	bool Render(float rotation);

private:
	bool RenderToTexture(float);
	bool RenderScene(float);
	bool RenderToTextureFade(float);
	bool RenderSceneFade(float );
	bool RenderFadingScene();
	bool RenderNormalScene(float);
	bool RenderRefractionToTexture();
	bool RenderReflectionToTexture();
	bool RenderSceneWater();
	bool RenderFire();
	bool RenderBillboard();
	bool RenderDepth();


	bool RenderBlurSceneToTexture(float);
	bool DownSampleTexture();
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();
	bool UpSampleTexture();
	bool Render2DTextureScene();
	bool RenderInstance();
	bool RenderTesellation();
	bool RenderParticle();

	bool RenderShadowDepth();
	bool RenderShadow();

	bool RenderMultiShadowDepth();
	bool RenderMultiShadowDepth2();
	bool RenderMultiShadow();
	bool RenderMultiShadow2();

	bool SoftShadowDownSampleTexture();
	bool RenderBlackAndWhiteShadows();
	bool SoftShadowRenderHorizontalBlurToTexture();
	bool SoftShadowRenderVerticalBlurToTexture();
	bool SoftShadowUpSampleTexture();
	bool RenderSoftShadow();

	bool RenderProjection();

	bool RenderGlow();
	bool RenderGlowMapToTexture();
	bool RenderUIElementsToTexture();
	bool RenderGlowScene();
	bool GlowDownSampleTexture();
	bool GlowRenderHorizontalBlurToTexture();
	bool GlowRenderVerticalBlurToTexture();
	bool GlowUpSampleTexture();

	bool RenderMultiShadowDepth3();

	bool RenderTransparentShadowDepth();
	bool RenderTransparentShadow();

	bool RenderDeferredTexture();
	bool RenderDeferred();


private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	//refract
	ModelNormalClass* m_Model = nullptr;
	ModelNormalClass* m_WindowModel = nullptr;
	RenderTextureClass* m_RenderTexture = nullptr;
	TextureShaderClass* m_TextureShader = nullptr;
	GlassShaderClass* m_GlassShader = nullptr;

	//Fire, alphablending
	ModelClass* m_ModelFire = nullptr;
	FireShaderClass* m_FireShader = nullptr;

	//billboard
	ModelClass2* m_FloorModel = nullptr;
	ModelBillboardClass* m_BillboardModel = nullptr;
	//depth
	DepthShaderClass* m_DepthShader = nullptr;

	//blur
	ModelClass2* m_BlurModel = nullptr;
	TextureShaderClass* m_BlurTextureShader = nullptr;
	HorizontalBlurShaderClass* m_HorizontalBlurShader = nullptr;
	VerticalBlurShaderClass* m_VerticalBlurShader = nullptr;
	RenderBlurTextureClass* m_BlurRenderTexture = nullptr;
	RenderBlurTextureClass* m_DownSampleTexure = nullptr;
	RenderBlurTextureClass* m_HorizontalBlurTexture = nullptr;
	RenderBlurTextureClass* m_VerticalBlurTexture = nullptr;
	RenderBlurTextureClass* m_UpSampleTexure = nullptr;
	OrthoWindowClass* m_SmallWindow = nullptr;
	OrthoWindowClass* m_FullScreenWindow = nullptr;

	//instance
	ModelInstanceClass* m_InstanceModel = nullptr;
	TextureInstanceShaderClass* m_InstanceTextureShader = nullptr;

	//Tesellation
	ModelTeselClass* m_TeselModel = nullptr;
	ColorShaderClass* m_ColorShader = nullptr;

	//Particle System
	ParticleSystemClass* m_ParticleSystem = nullptr;

	//Shadow
	ModelClass2* m_ShadowCubeModel = nullptr;
	ModelClass2* m_ShadowGroundModel = nullptr;
	ModelClass2* m_ShadowSphereModel = nullptr;
	LightClass* m_ShadowLight = nullptr;
	RenderBlurTextureClass* m_ShadowRenderTexture = nullptr;
	DepthShaderClass* m_ShadowDepthShader = nullptr;
	ShadowShaderClass* m_ShadowShader = nullptr;
	
	//MultiShadow
	LightClass* m_ShadowLight2 = nullptr;
	RenderBlurTextureClass* m_ShadowRenderTexture2 = nullptr;
	ShadowMultiShaderClass* m_ShadowMultiShader = nullptr;

	//SoftShadow
	RenderBlurTextureClass* m_BlackWhiteRenderTexture = nullptr;
	ShadowBlackWhiteShaderClass* m_ShadowBlackWhiteShader = nullptr;
	SoftShadowShaderClass* m_SoftShadowShader = nullptr;

	//Projection
	ModelClass2* m_ProGroundModel = nullptr;
	ModelClass2* m_ProCubeModel = nullptr;
	LightClass* m_ProLight = nullptr;
	ProjectionShaderClass* m_ProjectionShader = nullptr;
	TextureClass* m_ProjectionTexture = nullptr;
	ViewPointClass* m_ViewPoint = nullptr;

	ShaderManagerClass* m_ShaderManager = nullptr;

	BitmapGlowClass* m_GlowBitmap = nullptr;
	RenderBlurTextureClass* m_GlowRenderTexture = nullptr;

	//directional shadow
	LightClass* m_ShadowLight3 = nullptr;
	RenderBlurTextureClass* m_ShadowRenderTexture3 = nullptr;

	//TransparentDepth
	TreeClass* m_Tree = nullptr;
	RenderBlurTextureClass* m_TransparentDepthRenderTexture = nullptr;

	//Deferred
	DeferredBuffersClass* m_DeferredBuffers = nullptr;

	GraphicsDatas* g_data = nullptr;

	//std::vector<LightClass>* m_Lights = nullptr;;
	/*
	std::vector<DirLightClass>
	std::vector<SpotLightClass>
	std::vector<PointLightClass>
	*/
};
