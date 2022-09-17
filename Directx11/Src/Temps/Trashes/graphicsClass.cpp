////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Systems/Graphics/D3DClass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "modelclass2.h"
#include "lightclass.h"
#include "ShaderClass/LightShaderClass.h"
#include "graphicsclass.h"
#include "rendertextureclass.h"
#include "debugwindowclass.h"
#include "ShaderClass/textureshaderclass.h"
#include "ShaderClass/SpecMapShaderClass.h"
#include "ShaderClass/Fogshaderclass.h"
#include "ShaderClass/ClipPlaneShaderClass.h"
#include "ShaderClass/TransparentShaderClass.h"
#include "ShaderClass/reflectionshaderclass.h"
#include "ShaderClass/TranslateShaderClass.h"
#include "BitmapClass_NoTexture.h"
#include "ShaderClass/FadeShaderClass.h"
#include "ShaderClass/refractionshaderclass.h"
#include "ShaderClass/watershaderclass.h"
#include "PointLightClass.h"
#include "ShaderClass/MultiLightShaderClass.h"
#include "ShaderClass/GlassShaderClass.h"
#include "ModelClass_normal.h"
#include "ShaderClass/FireShaderClass.h"
#include "ModelBillboardClass.h"
#include "ShaderClass/DepthShaderClass.h"

#include "RenderBlurTextureClass.h"
#include "horizontalblurshaderclass.h"
#include "ShaderClass/verticalblurshaderclass.h"
#include "orthowindowclass.h"

#include "ModelInstanceClass.h"
#include "ShaderClass/TextureInstanceShaderClass.h"

#include "ModelTeselClass.h"
#include "ShaderClass/TessellationShaderClass.h"

#include "ParticleSystemClass.h"

#include "ShaderClass/ShadowShaderClass.h"
#include "ShaderClass/ShadowMultiShaderClass.h"

#include "ShaderClass/SoftShadowShaderClass.h"
#include "ShaderClass/ShadowBlackWhiteShaderClass.h"

#include "textureclass.h"
#include "ViewpointClass.h"
#include "ShaderClass/ProjectionShaderClass.h"
#include "ShaderClass/shadermanagerclass.h"

#include "ShaderClass/BitmapGlowClass.h"

#include "treeclass.h"

#include "DeferredBuffersClass.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx11.h"
#include "Imgui/imgui_impl_win32.h"

#include <vector>
#include "Systems/Application/Application.h"


GraphicsClass::GraphicsClass()
{
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	//D3D 객체 생성
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}
	bool result = false;

	//initialize
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, mAPPLICATION->GetHWND(),
		FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Direct3D 객체생성 오류", L"Error", MB_OK);
		return false;
	}

	// 크기를 샘플로 설정합니다.
	int downSampleWidth = screenWidth / 2;
	int downSampleHeight = screenHeight / 2;


	//camera create
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}
	// 카메라 포지션 설정
	XMMATRIX baseViewMatrix;
	m_Camera->SetPosition(0.0f, 2.0f, -10.0f);
	m_Camera->RenderBaseViewMatrix();
	//m_Camera->Render();
	//m_Camera->GetViewMatrix(baseViewMatrix);



	// 모델 객체 생성
	m_Model = new ModelNormalClass;
	if (!m_Model)
	{
		return false;
	}

	// 모델 객체 초기화
	if (!m_Model->Initialize(m_Direct3D->GetDevice(), "data/cube.txt", L"data/seafloor.dds", L"data/bump03.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// 창 모델 객체를 만듭니다.
	m_WindowModel = new ModelNormalClass;
	if (!m_WindowModel)
	{
		return false;
	}

#define GLASS	true	// t유리, f얼음

	// 창 모델 객체를 초기화합니다.	
#if GLASS
	if (!m_WindowModel->Initialize(m_Direct3D->GetDevice(), "data/square.txt", L"data/glass01.dds", L"data/bump03.dds"))	// 유리
#else
	if (!m_WindowModel->Initialize(m_Direct3D->GetDevice(), "data/square.txt", L"data/ice01.dds", L"data/icebump01.dds"))	// 얼음
#endif
	{
		MessageBox(hwnd, L"Could not initialize the window model object.", L"Error", MB_OK);
		return false;
	}

	// 렌더링 텍스처 객체를 생성한다.
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	// 렌더링 텍스처 객체를 초기화한다.
	if (!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight))
	{
		return false;
	}

	// 텍스처 쉐이더 객체를 생성한다.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// 텍스처 쉐이더 객체를 초기화한다.
	if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// 유리 쉐이더 객체를 만듭니다.
	m_GlassShader = new GlassShaderClass;
	if (!m_GlassShader)
	{
		return false;
	}

	// 유리 쉐이더 객체를 초기화합니다.
	if (!m_GlassShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the glass shader object.", L"Error", MB_OK);
		return false;
	}

	// 모델 객체 생성
	m_ModelFire = new ModelClass;
	if (!m_ModelFire)
	{
		return false;
	}

	// 모델 객체 초기화
	if (!m_ModelFire->Initialize(m_Direct3D->GetDevice(), "data/square.txt", L"data/fire01.dds",
		L"data/noise01.dds", L"data/alpha01.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// 파이어 쉐이더 객체를 생성한다.
	m_FireShader = new FireShaderClass;
	if (!m_FireShader)
	{
		return false;
	}

	// 화재 쉐이더 객체를 초기화합니다.
	if (!m_FireShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the fire shader object.", L"Error", MB_OK);
		return false;
	}

	// 바닥 모델 객체를 만듭니다.
	m_FloorModel = new ModelClass2;
	if (!m_FloorModel)
	{
		return false;
	}

	// 바닥 모델 객체를 초기화합니다.
	if (!m_FloorModel->Initialize(m_Direct3D->GetDevice(), L"data/grid01.dds", "data/floor.txt"))
	{
		MessageBox(hwnd, L"Could not initialize the floor model object.", L"Error", MB_OK);
		return false;
	}

	// 빌보드 모델 객체를 만듭니다.
	m_BillboardModel = new ModelBillboardClass;
	if (!m_BillboardModel)
	{
		return false;
	}

	// 빌보드 모델 객체를 초기화합니다.
	if (!m_BillboardModel->Initialize(m_Direct3D->GetDevice(), "data/square.txt", L"data/seafloor.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the billboard model object.", L"Error", MB_OK);
		return false;
	}

	// 깊이 셰이더 개체를 만듭니다.
	m_DepthShader = new DepthShaderClass;
	if (!m_DepthShader)
	{
		return false;
	}

	// 깊이 셰이더 개체를 초기화합니다.
	if (!m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
		return false;
	}

	// 모델 객체 생성
	m_BlurModel = new ModelClass2;
	if (!m_BlurModel)
	{
		return false;
	}

	// 모델 객체 초기화
	result = m_BlurModel->Initialize(m_Direct3D->GetDevice(), L"data/seafloor.dds", "data/cube.txt" );
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// 텍스처 쉐이더 객체를 생성한다.
	m_BlurTextureShader = new TextureShaderClass;
	if (!m_BlurTextureShader)
	{
		return false;
	}

	// 텍스처 쉐이더 객체를 초기화한다.
	result = m_BlurTextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// 수평 블러 쉐이더 객체를 만듭니다.
	m_HorizontalBlurShader = new HorizontalBlurShaderClass;
	if (!m_HorizontalBlurShader)
	{
		return false;
	}

	// 수평 블러 쉐이더 객체를 초기화합니다.
	result = m_HorizontalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur shader object.", L"Error", MB_OK);
		return false;
	}

	// 수직 블러 셰이더 오브젝트를 생성한다.
	m_VerticalBlurShader = new VerticalBlurShaderClass;
	if (!m_VerticalBlurShader)
	{
		return false;
	}

	// 수직 블러 쉐이더 객체를 초기화합니다.
	result = m_VerticalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur shader object.", L"Error", MB_OK);
		return false;
	}

	// 렌더링 텍스처 객체를 생성한다.
	m_BlurRenderTexture = new RenderBlurTextureClass;
	if (!m_BlurRenderTexture)
	{
		return false;
	}

	// 렌더링 텍스처 객체를 초기화한다.
	result = m_BlurRenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// 다운 샘플 렌더링을 텍스처 오브젝트에 생성한다.
	m_DownSampleTexure = new RenderBlurTextureClass;
	if (!m_DownSampleTexure)
	{
		return false;
	}

	// 다운 샘플 렌더를 텍스처 오브젝트로 초기화한다.
	result = m_DownSampleTexure->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the down sample render to texture object.", L"Error", MB_OK);
		return false;
	}

	// 텍스처 객체에 수평 블러 렌더링을 만듭니다.
	m_HorizontalBlurTexture = new RenderBlurTextureClass;
	if (!m_HorizontalBlurTexture)
	{
		return false;
	}

	// 텍스처 객체에 수평 블러 렌더링을 초기화합니다.
	result = m_HorizontalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// 텍스처 오브젝트에 수직 블러 렌더를 만듭니다.
	m_VerticalBlurTexture = new RenderBlurTextureClass;
	if (!m_VerticalBlurTexture)
	{
		return false;
	}

	// 텍스처 오브젝트에 수직 블러 렌더를 초기화합니다.
	result = m_VerticalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// 텍스쳐 객체에 업 샘플 렌더를 생성한다.
	m_UpSampleTexure = new RenderBlurTextureClass;
	if (!m_UpSampleTexure)
	{
		return false;
	}

	// up 샘플 렌더를 텍스처 오브젝트로 초기화한다.
	result = m_UpSampleTexure->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the up sample render to texture object.", L"Error", MB_OK);
		return false;
	}

	// 작은 ortho 윈도우 객체를 만듭니다.
	m_SmallWindow = new OrthoWindowClass;
	if (!m_SmallWindow)
	{
		return false;
	}

	// 작은 ortho 윈도우 객체를 초기화합니다.
	result = m_SmallWindow->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the small ortho window object.", L"Error", MB_OK);
		return false;
	}

	// 전체 화면 ortho window 객체를 만듭니다.
	m_FullScreenWindow = new OrthoWindowClass;
	if (!m_FullScreenWindow)
	{
		return false;
	}

	// 전체 화면 ortho window 객체를 초기화합니다.
	result = m_FullScreenWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
		return false;
	}


	// m_Model 객체 생성
	m_InstanceModel = new ModelInstanceClass;
	if (!m_InstanceModel)
	{
		return false;
	}

	// m_Model 객체 초기화
	if (!m_InstanceModel->Initialize(m_Direct3D->GetDevice(), L"data/seafloor.dds", "data/cube.txt"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// m_TextureShader 객체 생성
	m_InstanceTextureShader = new TextureInstanceShaderClass;
	if (!m_InstanceTextureShader)
	{
		return false;
	}

	// m_TextureShader 객체 초기화
	if (!m_InstanceTextureShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}


	// m_Model 객체 생성
	m_TeselModel = new ModelTeselClass;
	if (!m_TeselModel)
	{
		return false;
	}

	// m_Model 객체 초기화
	if (!m_TeselModel->Initialize(m_Direct3D->GetDevice()))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// m_ColorShader 객체 생성
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
	{
		return false;
	}

	// m_ColorShader 객체 초기화
	if (!m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// 파티클 시스템 객체를 만듭니다.
	m_ParticleSystem = new ParticleSystemClass;
	if (!m_ParticleSystem)
	{
		return false;
	}

	// 파티클 시스템 객체를 초기화합니다.
	if (!m_ParticleSystem->Initialize(m_Direct3D->GetDevice(), L"data/star.dds"))
	{
		return false;
	}

	// 큐브 모델 오브젝트를 생성합니다.
	m_ShadowCubeModel = new ModelClass2;
	if (!m_ShadowCubeModel)
	{
		return false;
	}

	// 큐브 모델 오브젝트를 초기화 합니다.
	result = m_ShadowCubeModel->Initialize(m_Direct3D->GetDevice(),L"data/wall01.dds", "data/cube.txt" );
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
		return false;
	}

	// 큐브 모델의 위치를 ​​설정 합니다.
	m_ShadowCubeModel->SetPosition(-2.0f, 2.0f, 0.0f);

	// 구형 모델 객체를 만듭니다.
	m_ShadowSphereModel = new ModelClass2;
	if (!m_ShadowSphereModel)
	{
		return false;
	}

	// 구형 모델 객체를 초기화합니다.
	result = m_ShadowSphereModel->Initialize(m_Direct3D->GetDevice(),  L"data/ice.dds", "data/sphere.txt" );
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the sphere model object.", L"Error", MB_OK);
		return false;
	}

	// 구형 모델의 위치를 ​​설정합니다.
	m_ShadowSphereModel->SetPosition(2.0f, 2.0f, 0.0f);

	// 지면 모델 객체를 만듭니다.
	m_ShadowGroundModel = new ModelClass2;
	if (!m_ShadowGroundModel)
	{
		return false;
	}

	// 지면 모델 객체를 초기화합니다.
	result = m_ShadowGroundModel->Initialize(m_Direct3D->GetDevice(), L"data/metal001.dds", "data/plane01.txt");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}

	// 지면 모델의 위치를 ​​설정합니다.
	m_ShadowGroundModel->SetPosition(0.0f, 1.0f, 0.0f);

	// light 객체를 만듭니다.
	m_ShadowLight = new LightClass;
	if (!m_ShadowLight)
	{
		return false;
	}

	// 조명 객체를 초기화합니다.
	m_ShadowLight->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_ShadowLight->SetDiffuseColor(0.0f, 0.0f, 1.0f, 1.0f);
	m_ShadowLight->SetLookAt(0.0f, 0.0f, 0.0f);
	m_ShadowLight->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);
	m_ShadowLight->GenerateOrthoMatrix(15.0f, 15.0f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);

	// 렌더링을 텍스처 오브젝트에 생성한다.
	m_ShadowRenderTexture = new RenderBlurTextureClass;
	if (!m_ShadowRenderTexture)
	{
		return false;
	}

	// 렌더링을 텍스처 오브젝트에 초기화한다.
	result = m_ShadowRenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// 깊이 셰이더 개체를 만듭니다.
	m_ShadowDepthShader = new DepthShaderClass;
	if (!m_ShadowDepthShader)
	{
		return false;
	}

	// 깊이 셰이더 개체를 초기화합니다.
	result = m_ShadowDepthShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
		return false;
	}

	// 그림자 셰이더 개체를 만듭니다.
	m_ShadowShader = new ShadowShaderClass;
	if (!m_ShadowShader)
	{
		return false;
	}

	// 그림자 쉐이더 객체를 초기화합니다.
	result = m_ShadowShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
		return false;
	}

	// 그림자 셰이더 개체를 만듭니다.
	m_ShadowMultiShader = new ShadowMultiShaderClass;
	if (!m_ShadowMultiShader)
	{
		return false;
	}

	// 그림자 쉐이더 객체를 초기화합니다.
	result = m_ShadowMultiShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
		return false;
	}

	// 두 번째 조명 객체를 만듭니다.
	m_ShadowLight2 = new LightClass;
	if (!m_ShadowLight2)
	{
		return false;
	}

	// 두 번째 조명 객체를 초기화합니다.
	m_ShadowLight2->SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f);
	m_ShadowLight2->SetLookAt(0.0f, 0.0f, 0.0f);
	m_ShadowLight2->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

	// 텍스처 오브젝트에 두 번째 렌더링을 만듭니다.
	m_ShadowRenderTexture2 = new RenderBlurTextureClass;
	if (!m_ShadowRenderTexture2)
	{
		return false;
	}

	// 텍스처 오브젝트에 두 번째 렌더를 초기화한다.
	result = m_ShadowRenderTexture2->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the second render to texture object.", L"Error", MB_OK);
		return false;
	}

	// light 객체를 만듭니다.
	m_ShadowLight3 = new LightClass;
	if (!m_ShadowLight3)
	{
		return false;
	}

	// 조명 객체를 초기화합니다.
	m_ShadowLight3->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_ShadowLight3->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	m_ShadowLight3->GenerateOrthoMatrix(20.0f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);

	// 렌더링을 텍스처 오브젝트에 생성한다.
	m_ShadowRenderTexture3 = new RenderBlurTextureClass;
	if (!m_ShadowRenderTexture3)
	{
		return false;
	}

	// 렌더링을 텍스처 오브젝트를 초기화한다.
	result = m_ShadowRenderTexture3->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// 그림자 셰이더 개체를 만듭니다.
	m_ShadowBlackWhiteShader = new ShadowBlackWhiteShaderClass;
	if (!m_ShadowBlackWhiteShader)
	{
		return false;
	}

	// 그림자 쉐이더 객체를 초기화합니다.
	result = m_ShadowBlackWhiteShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
		return false;
	}

	// 흑백 렌더링을 텍스처 오브젝트에 생성한다.
	m_BlackWhiteRenderTexture = new RenderBlurTextureClass;
	if (!m_BlackWhiteRenderTexture)
	{
		return false;
	}

	// 흑백 렌더링을 텍스처 오브젝트에 초기화한다.
	result = m_BlackWhiteRenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the black and white render to texture object.", L"Error", MB_OK);
		return false;
	}

	// 부드러운 그림자 쉐이더 객체를 만듭니다.
	m_SoftShadowShader = new SoftShadowShaderClass;
	if (!m_SoftShadowShader)
	{
		return false;
	}

	// 부드러운 그림자 쉐이더 객체를 초기화합니다.
	result = m_SoftShadowShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the soft shadow shader object.", L"Error", MB_OK);
		return false;
	}

	/************************************************************************************************************
	*  Projection
	**************************************************************************************************************/
	// 그라운드 모델 객체를 만듭니다.
	m_ProGroundModel = new ModelClass2;
	if (!m_ProGroundModel)
	{
		return false;
	}

	// 지면 모델 객체를 초기화합니다.
	result = m_ProGroundModel->Initialize(m_Direct3D->GetDevice(), L"data/stone01.dds", "data/floor.txt");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}


	// 큐브 모델 객체를 생성합니다.
	m_ProCubeModel = new ModelClass2;
	if (!m_ProCubeModel)
	{
		return false;
	}

	// 큐브 모델 객체를 초기화 합니다.
	result = m_ProCubeModel->Initialize(m_Direct3D->GetDevice(), L"data/seafloor.dds", "data/cube.txt");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
		return false;
	}

	// 조명 객체를 생성합니다.
	m_ProLight = new LightClass;
	if (!m_ProLight)
	{
		return false;
	}

	// 조명 객체를 초기화 합니다.
	m_ProLight->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_ProLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_ProLight->SetDirection(0.0f, -0.75f, 0.5f);
	m_ProLight->SetDirection(1.0f, -1.0f, 1.0f);

	// 프로젝션 셰이더 개체를 만듭니다.
	m_ProjectionShader = new ProjectionShaderClass;
	if (!m_ProjectionShader)
	{
		return false;
	}

	// 프로젝션 셰이더 개체를 초기화합니다.
	result = m_ProjectionShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the projection shader object.", L"Error", MB_OK);
		return false;
	}

	// 투영 텍스처 객체를 만듭니다.
	m_ProjectionTexture = new TextureClass;
	if (!m_ProjectionTexture)
	{
		return false;
	}

	// 투영 텍스처 객체를 초기화합니다.
	result = m_ProjectionTexture->Initialize(m_Direct3D->GetDevice(), L"data/dx11.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the projection texture object.", L"Error", MB_OK);
		return false;
	}

	// 뷰 포인트 객체를 만듭니다.
	m_ViewPoint = new ViewPointClass;
	if (!m_ViewPoint)
	{
		return false;
	}

	// 뷰 포인트 객체를 초기화합니다.
	m_ViewPoint->SetPosition(2.0f, 5.0f, -2.0f);
	m_ViewPoint->SetLookAt(0.0f, 0.0f, 0.0f);
	m_ViewPoint->SetProjectionParameters((float)(XM_PI / 2.0f), 1.0f, 0.1f, 100.0f);
	m_ViewPoint->GenerateViewMatrix();
	m_ViewPoint->GenerateProjectionMatrix();

	// 셰이더 관리자 객체를 만듭니다.
	m_ShaderManager = new ShaderManagerClass;
	if (!m_ShaderManager)
	{
		return false;
	}

	// 셰이더 관리자 객체를 초기화합니다.
	result = m_ShaderManager->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the shader manager object.", L"Error", MB_OK);
		return false;
	}

	// 비트 맵 객체를 만듭니다.
	m_GlowBitmap = new BitmapGlowClass;
	if (!m_GlowBitmap)
	{
		return false;
	}

	// 비트 맵 객체를 초기화합니다.
	result = m_GlowBitmap->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, L"data/test.dds", L"data/glowmap.dds", 256, 32);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}
	// 렌더링 텍스처 객체를 생성한다.
	m_GlowRenderTexture = new RenderBlurTextureClass;
	if (!m_GlowRenderTexture)
	{
		return false;
	}

	// 렌더링 텍스처 객체를 초기화한다.
	result = m_GlowRenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}


	// 나무 객체를 만듭니다.
	m_Tree = new TreeClass;
	if (!m_Tree)
	{
		return false;
	}

	// 그림자 셰이더 개체를 초기화합니다.
	result = m_Tree->Initialize(m_Direct3D->GetDevice(), "data/trees/trunk001.txt", L"data/trees/trunk001.dds", "data/trees/leaf001.txt", L"data/trees/leaf001.dds", 0.1f);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the tree object.", L"Error", MB_OK);
		return false;
	}

	// 나무 모델의 위치를 ​​설정합니다.
	m_Tree->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));

	// 텍스처 객체에 렌더링을 만듭니다.
	m_TransparentDepthRenderTexture = new RenderBlurTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	// 렌더링을 텍스처 객체로 초기화합니다.
	result = m_TransparentDepthRenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// 지연된 버퍼 객체를 생성합니다.
	m_DeferredBuffers = new DeferredBuffersClass;
	if (!m_DeferredBuffers)
	{
		return false;
	}

	// 지연된 버퍼 객체를 초기화 합니다.
	result = m_DeferredBuffers->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the deferred buffers object.", L"Error", MB_OK);
		return false;
	}

	g_data = new GraphicsDatas;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
	ImGui::StyleColorsDark();
	return true;
}


void GraphicsClass::Shutdown()
{
	// 지연된 버퍼 객체를 해제합니다.
	if (m_DeferredBuffers)
	{
		m_DeferredBuffers->Shutdown();
		delete m_DeferredBuffers;
		m_DeferredBuffers = 0;
	}

	// 렌더 투 텍스쳐 객체를 해제합니다.
	if (m_TransparentDepthRenderTexture)
	{
		m_TransparentDepthRenderTexture->Shutdown();
		delete m_TransparentDepthRenderTexture;
		m_TransparentDepthRenderTexture = 0;
	}

	// 나무 객체를 해제합니다.
	if (m_Tree)
	{
		m_Tree->Shutdown();
		delete m_Tree;
		m_Tree = 0;
	}
	// 렌더 투 텍스쳐 객체를 해제합니다.
	if (m_ShadowRenderTexture3)
	{
		m_ShadowRenderTexture3->Shutdown();
		delete m_ShadowRenderTexture3;
		m_ShadowRenderTexture3 = 0;
	}

	// 조명 객체를 해제합니다.
	if (m_ShadowLight3)
	{
		delete m_ShadowLight3;
		m_ShadowLight3 = 0;
	}

	// 렌더를 텍스쳐 객체로 릴리즈한다.
	if (m_GlowRenderTexture)
	{
		m_GlowRenderTexture->Shutdown();
		delete m_GlowRenderTexture;
		m_GlowRenderTexture = 0;
	}

	// 비트 맵 객체를 해제합니다.
	if (m_GlowBitmap)
	{
		m_GlowBitmap->Shutdown();
		delete m_GlowBitmap;
		m_GlowBitmap = 0;
	}

	// 셰이더 관리자 객체를 해제합니다.
	if (m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}

	// 뷰 포인트 객체를 해제합니다.
	if (m_ViewPoint)
	{
		delete m_ViewPoint;
		m_ViewPoint = 0;
	}

	// 투영 텍스처 객체를 해제합니다.
	if (m_ProjectionTexture)
	{
		m_ProjectionTexture->Shutdown();
		delete m_ProjectionTexture;
		m_ProjectionTexture = 0;
	}

	// 투영 쉐이더 객체를 해제합니다.
	if (m_ProjectionShader)
	{
		m_ProjectionShader->Shutdown();
		delete m_ProjectionShader;
		m_ProjectionShader = 0;
	}

	// light 오브젝트를 해제한다.
	if (m_ProLight)
	{
		delete m_ProLight;
		m_ProLight = 0;
	}

	// 큐브 모델 오브젝트를 해제하십시오.
	if (m_ProCubeModel)
	{
		m_ProCubeModel->Shutdown();
		delete m_ProCubeModel;
		m_ProCubeModel = 0;
	}

	// 그라운드 모델 객체를 해제한다.
	if (m_ProGroundModel)
	{
		m_ProGroundModel->Shutdown();
		delete m_ProGroundModel;
		m_ProGroundModel = 0;
	}

	// 부드러운 그림자 쉐이더 객체를 해제한다.
	if (m_SoftShadowShader)
	{
		m_SoftShadowShader->Shutdown();
		delete m_SoftShadowShader;
		m_SoftShadowShader = 0;
	}

	// 그림자 쉐이더 객체를 해제합니다.
	if (m_ShadowBlackWhiteShader)
	{
		m_ShadowBlackWhiteShader->Shutdown();
		delete m_ShadowBlackWhiteShader;
		m_ShadowBlackWhiteShader = 0;
	}

	// 흑백 렌더링을 텍스처로 놓습니다.
	if (m_BlackWhiteRenderTexture)
	{
		m_BlackWhiteRenderTexture->Shutdown();
		delete m_BlackWhiteRenderTexture;
		m_BlackWhiteRenderTexture = 0;
	}


	// 두 번째 렌더를 텍스처 객체로 해제합니다.
	if (m_ShadowRenderTexture2)
	{
		m_ShadowRenderTexture2->Shutdown();
		delete m_ShadowRenderTexture2;
		m_ShadowRenderTexture2 = 0;
	}

	// 두 번째 조명 객체를 해제합니다.
	if (m_ShadowLight2)
	{
		delete m_ShadowLight2;
		m_ShadowLight2 = 0;
	}

	// 그림자 쉐이더 객체를 해제합니다.
	if (m_ShadowMultiShader)
	{
		m_ShadowMultiShader->Shutdown();
		delete m_ShadowMultiShader;
		m_ShadowMultiShader = 0;
	}

	// 그림자 쉐이더 객체를 해제합니다.
	if (m_ShadowShader)
	{
		m_ShadowShader->Shutdown();
		delete m_ShadowShader;
		m_ShadowShader = 0;
	}

	// 깊이 셰이더 개체를 해제합니다.
	if (m_ShadowDepthShader)
	{
		m_ShadowDepthShader->Shutdown();
		delete m_ShadowDepthShader;
		m_ShadowDepthShader = 0;
	}

	// 렌더 투 텍스쳐 객체를 해제합니다.
	if (m_ShadowRenderTexture)
	{
		m_ShadowRenderTexture->Shutdown();
		delete m_ShadowRenderTexture;
		m_ShadowRenderTexture = 0;
	}

	// 조명 객체를 해제합니다.
	if (m_ShadowLight)
	{
		delete m_ShadowLight;
		m_ShadowLight = 0;
	}

	// 지면 모델 객체를 해제합니다.
	if (m_ShadowGroundModel)
	{
		m_ShadowGroundModel->Shutdown();
		delete m_ShadowGroundModel;
		m_ShadowGroundModel = 0;
	}

	// 구형 모델 객체를 해제합니다.
	if (m_ShadowSphereModel)
	{
		m_ShadowSphereModel->Shutdown();
		delete m_ShadowSphereModel;
		m_ShadowSphereModel = 0;
	}

	// 큐브 모델 객체를 해제합니다.
	if (m_ShadowCubeModel)
	{
		m_ShadowCubeModel->Shutdown();
		delete m_ShadowCubeModel;
		m_ShadowCubeModel = 0;
	}

	// 파티클 시스템 객체를 해제합니다.
	if (m_ParticleSystem)
	{
		m_ParticleSystem->Shutdown();
		delete m_ParticleSystem;
		m_ParticleSystem = 0;
	}

	// m_ColorShader 객체 반환
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// m_Model 객체 반환
	if (m_TeselModel)
	{
		m_TeselModel->Shutdown();
		delete m_TeselModel;
		m_TeselModel = 0;
	}

	// m_InstanceTextureShader 객체 반환
	if (m_InstanceTextureShader)
	{
		m_InstanceTextureShader->Shutdown();
		delete m_InstanceTextureShader;
		m_InstanceTextureShader = 0;
	}

	// m_Model 객체 반환
	if (m_InstanceModel)
	{
		m_InstanceModel->Shutdown();
		delete m_InstanceModel;
		m_InstanceModel = 0;
	}
	// 전체 화면 ortho window 객체를 해제합니다.
	if (m_FullScreenWindow)
	{
		m_FullScreenWindow->Shutdown();
		delete m_FullScreenWindow;
		m_FullScreenWindow = 0;
	}

	// 작은 ortho 윈도우 객체를 놓습니다.
	if (m_SmallWindow)
	{
		m_SmallWindow->Shutdown();
		delete m_SmallWindow;
		m_SmallWindow = 0;
	}

	// up 샘플 렌더를 텍스쳐 객체로 릴리즈한다.
	if (m_UpSampleTexure)
	{
		m_UpSampleTexure->Shutdown();
		delete m_UpSampleTexure;
		m_UpSampleTexure = 0;
	}

	// 수직 블러 렌더를 텍스처 객체에 놓습니다.
	if (m_VerticalBlurTexture)
	{
		m_VerticalBlurTexture->Shutdown();
		delete m_VerticalBlurTexture;
		m_VerticalBlurTexture = 0;
	}

	// 수평 블러 렌더를 텍스처 객체에 놓습니다.
	if (m_HorizontalBlurTexture)
	{
		m_HorizontalBlurTexture->Shutdown();
		delete m_HorizontalBlurTexture;
		m_HorizontalBlurTexture = 0;
	}

	// 다운 샘플 렌더를 텍스쳐 객체로 릴리즈한다.
	if (m_DownSampleTexure)
	{
		m_DownSampleTexure->Shutdown();
		delete m_DownSampleTexure;
		m_DownSampleTexure = 0;
	}

	// 수직 블러 쉐이더 객체를 해제한다.
	if (m_VerticalBlurShader)
	{
		m_VerticalBlurShader->Shutdown();
		delete m_VerticalBlurShader;
		m_VerticalBlurShader = 0;
	}

	// 수평 블러 쉐이더 객체를 해제합니다.
	if (m_HorizontalBlurShader)
	{
		m_HorizontalBlurShader->Shutdown();
		delete m_HorizontalBlurShader;
		m_HorizontalBlurShader = 0;
	}

	// 렌더를 텍스쳐 객체로 릴리즈한다.
	if (m_BlurRenderTexture)
	{
		m_BlurRenderTexture->Shutdown();
		delete m_BlurRenderTexture;
		m_BlurRenderTexture = 0;
	}

	// Release the vertical blur shader object.
	if (m_VerticalBlurShader)
	{
		m_VerticalBlurShader->Shutdown();
		delete m_VerticalBlurShader;
		m_VerticalBlurShader = 0;
	}

	// Release the horizontal blur shader object.
	if (m_HorizontalBlurShader)
	{
		m_HorizontalBlurShader->Shutdown();
		delete m_HorizontalBlurShader;
		m_HorizontalBlurShader = 0;
	}

	// Release the texture shader object.
	if (m_BlurTextureShader)
	{
		m_BlurTextureShader->Shutdown();
		delete m_BlurTextureShader;
		m_BlurTextureShader = 0;
	}

	// 모델 객체 반환
	if (m_BlurModel)
	{
		m_BlurModel->Shutdown();
		delete m_BlurModel;
		m_BlurModel = 0;
	}


	// 깊이 셰이더 객체 반환
	if (m_DepthShader)
	{
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}

	// fire shader 객체를 해제한다.
	if (m_FireShader)
	{
		m_FireShader->Shutdown();
		delete m_FireShader;
		m_FireShader = 0;
	}

	// 모델 객체를 해제합니다.
	if (m_ModelFire)
	{
		m_ModelFire->Shutdown();
		delete m_ModelFire;
		m_ModelFire = 0;
	}

	// 모델 객체 반환
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// 텍스처 쉐이더 객체를 해제한다.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// 렌더를 텍스쳐 객체로 릴리즈한다.
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}


	// m_Camera 객체 반환
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	//D3D 객체 반환
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = NULL;
	}
	return;
}


bool GraphicsClass::Frame(float frameTime, XMFLOAT3& position)
{
	// Set the position and rotation of the camera.
	//m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	m_Camera->SetPosition(position);

	static float rotation = 0.0f;

	//update rotation
	rotation += (float)XM_PI * 0.002f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	m_ParticleSystem->Frame(frameTime, m_Direct3D->GetDeviceContext());

	static float lightPositionX = -5.0f;

	// 각 프레임의 조명 위치를 업데이트합니다.
	lightPositionX += 0.05f;
	if (lightPositionX > 5.0f)
	{
		lightPositionX = -5.0f;
	}

	// 빛의 위치를 ​​업데이트합니다.
	m_ShadowLight->SetPosition(lightPositionX, 8.0f, 1.0f);

	// 두 번째 빛의 위치를 ​​설정합니다.
	m_ShadowLight2->SetPosition(-5.0f, 8.0f, -5.0f);

	static float lightAngle = 270.0f;
	static float lightPosX = 9.0f;

	// 각 프레임의 조명 위치를 업데이트 합니다.
	lightPosX -= 0.003f * frameTime;

	// 각 프레임의 조명 각도를 업데이트 합니다.
	lightAngle -= 0.03f * frameTime;
	if (lightAngle < 90.0f)
	{
		lightAngle = 270.0f;

		// 조명 위치도 재설정 합니다.
		lightPosX = 9.0f;
	}
	float radians = lightAngle * 0.0174532925f;

	// 조회 및 위치를 업데이트 합니다.
	m_ShadowLight->SetPosition(0.0f + lightPosX, 10.0f, 1.0f);
	m_ShadowLight->SetLookAt(0.0f - lightPosX, 0.0f, 2.0f);

	// 빛의 방향을 업데이 트합니다.
	m_ShadowLight->SetDirection(sinf(radians), cosf(radians), 0.0f);
	m_ShadowLight3->SetDirection(sinf(radians), cosf(radians), 0.0f);

	// 빛의 위치와 표시를 설정합니다.
	m_ShadowLight3->SetPosition(lightPosX, 8.0f, -0.1f);
	m_ShadowLight3->SetLookAt(-lightPosX, 0.0f, 0.0f);
	return Render(rotation);
}

bool GraphicsClass::Render(float rotation)
{
	bool result = false;
	bool Blur = false;
	bool Billboard = false;
	bool Fire = false;
	bool Refraction = false;
	bool Depth = false;
	bool Instancing = false;
	bool Tesellation = false;
	bool particlesystem = false;
	bool Shadow = false;
	bool MultiShadow = false;
	bool SoftShadow = false;
	bool Projection = false;
	bool Glow = false;
	bool TransparentShadow = true;
	bool Deferred = true;

	ImVec4 clearVec4 = ImVec4(g_data->clear_color[0], g_data->clear_color[1], g_data->clear_color[2], 1.0f);
	// 장면을 시작할 버퍼를 지운다.
	m_Direct3D->BeginScene(clearVec4.x, clearVec4.y, clearVec4.z, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	//m_Camera->Render();

	if (Refraction)
	{
		if (!RenderToTexture(rotation))
		{
			return false;
		}

		 //장면을 렌더링 합니다.
		if (!RenderScene(rotation))
		{
			return false;
		}
	}

	if (Billboard)
	{
		if (!RenderBillboard())
		{
			return false;
		}
	}

	if (Fire)
	{
		if (!RenderFire())
		{
			return false;
		}
	}

	if (Depth)
	{
		if (!RenderDepth())
		{
			return false;
		}
	}

	//Blur
	if(Blur)
	{
		// 전체 장면을 먼저 텍스처로 렌더링합니다.
		result = RenderBlurSceneToTexture(rotation);
		if (!result)
		{
			return false;
		}

		// 다음으로 렌더 텍스처를 작은 크기의 텍스처로 샘플링합니다.
		result = DownSampleTexture();
		if (!result)
		{
			return false;
		}

		// 다운 샘플링 된 렌더링 텍스처에 수평 블러를 수행합니다.
		result = RenderHorizontalBlurToTexture();
		if (!result)
		{
			return false;
		}

		// 이제 수평 흐림 렌더 텍스처에서 수직 흐림 효과를 수행합니다.
		result = RenderVerticalBlurToTexture();
		if (!result)
		{
			return false;
		}

		// 최종 흐린 렌더링 텍스처를 화면 크기로 다시 샘플링합니다.
		result = UpSampleTexture();
		if (!result)
		{
			return false;
		}

		// 흐리게 샘플링 된 렌더링 텍스처를 화면에 렌더링합니다.
		result = Render2DTextureScene();
		if (!result)
		{
			return false;
		}
	}

	if (Instancing)
	if (!RenderInstance())
	{
		return false;
	}

	if (Tesellation)
	{
		if (!RenderTesellation()) return false;
	}

	if (particlesystem)
	{
		if (!RenderParticle()) return false;
	}

	if (Shadow)
	{
		if (!RenderShadow()) return false;
	}

	if (MultiShadow)
	{
		if (!RenderMultiShadow2()) return false;
	}

	if (SoftShadow)
	{
		if (!RenderSoftShadow()) return false;
	}


	if (Projection)
	{
		if (!RenderProjection()) return false;
	}

	if (Glow)
	{
		if (!RenderGlow()) return false;

	}

	if (TransparentShadow)
	{
		if (!RenderTransparentShadow()) return false;
	}

	if (g_data->Deferred)
	{
		if (!RenderDeferred()) return false;
	}


	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Hello, world!");
	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	ImGui::Checkbox("Deferred", &g_data->Deferred);      // Edit bools storing our window open/close state

	ImGui::SliderFloat("float", &g_data->f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::ColorEdit3("clear color", (float*)&clearVec4); // Edit 3 floats representing a color
	g_data->clear_color[0] = clearVec4.x;
	g_data->clear_color[1] = clearVec4.y;
	g_data->clear_color[2] = clearVec4.z;
	ImGui::SliderFloat3("position", g_data->firePosition, -5.0f, 5.0f);
	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		g_data->counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", g_data->counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();

	// 3. Show another simple window.
	if (g_data->show_another_window)
	{
		ImGui::Begin("Another Window", &g_data->show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			g_data->show_another_window = false;
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_Direct3D->EndScene();
	return true;

}


bool GraphicsClass::RenderToTexture(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());

	// 렌더링을 텍스처에 지 웁니다.
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 회전에 의해 월드 행렬에 곱합니다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더로 모델을 렌더링한다.
	m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_Model->GetTexture());

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}


bool GraphicsClass::RenderScene(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// 유리 쉐이더의 굴절 스케일을 설정합니다.
#if GLASS
	float refractionScale = 0.01f;	// 유리 효과
#else
	float refractionScale = 0.1f;	// 얼음 효과
#endif

	// 장면을 시작할 버퍼를 지운다.
	//m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 오쏘 (ortho) 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 회전에 의해 월드 행렬에 곱합니다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더로 모델을 렌더링한다.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_Model->GetTexture()))
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 윈도우 모델이 렌더링 될 곳으로 되돌립니다.
	worldMatrix = XMMatrixTranslation(0.0f, 0.0f, -1.5f);

	// 창 모델 정점과 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	m_WindowModel->Render(m_Direct3D->GetDeviceContext());

	// 유리 쉐이더를 사용하여 창 모델을 렌더링합니다.
	if (!m_GlassShader->Render(m_Direct3D->GetDeviceContext(), m_WindowModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_WindowModel->GetTexture(), m_WindowModel->GetNormalMap(), m_RenderTexture->GetShaderResourceView(),
		refractionScale))
	{
		return false;
	}

	// 렌더링 된 장면을 화면에 표시합니다.
	//m_Direct3D->EndScene();

	return true;
}


bool GraphicsClass::RenderFire()
{
	static float frameTime = 0.0f;


	// 프레임 시간 카운터를 증가시킵니다.
	frameTime += 0.01f;
	if (frameTime > 1000.0f)
	{
		frameTime = 0.0f;
	}

	// 세 가지 다른 노이즈 텍스처에 대해 세 가지 스크롤 속도를 설정합니다.
	XMFLOAT3 scrollSpeeds = XMFLOAT3(1.3f, 2.1f, 2.3f);

	// 세 개의 서로 다른 노이즈 옥타브 텍스처를 만드는 데 사용할 세 개의 스케일을 설정합니다.
	XMFLOAT3 scales = XMFLOAT3(1.0f, 2.0f, 3.0f);

	// 세 가지 다른 노이즈 텍스처에 대해 세 가지 다른 x 및 y 왜곡 인수를 설정합니다.
	XMFLOAT2 distortion1 = XMFLOAT2(0.1f, 0.2f);
	XMFLOAT2 distortion2 = XMFLOAT2(0.1f, 0.3f);
	XMFLOAT2 distortion3 = XMFLOAT2(0.1f, 0.1f);

	// 텍스처 좌표 샘플링 섭동의 스케일과 바이어스.
	float distortionScale = 0.8f;
	float distortionBias = 0.5f;

	// 장면을 시작할 버퍼를 지운다.
	//m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 오쏘 (ortho) 행렬을 가져옵니다.
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 화재 투명도의 알파 블렌드를 켭니다.
	m_Direct3D->TurnOnAlphaBlending();

	// 정사각형 모델의 정점과 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	m_ModelFire->Render(m_Direct3D->GetDeviceContext());

	// 화재 쉐이더를 사용하여 사각형 모델을 렌더링합니다.
	if (!m_FireShader->Render(m_Direct3D->GetDeviceContext(), m_ModelFire->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_ModelFire->GetTexture(1), m_ModelFire->GetTexture(2), m_ModelFire->GetTexture(3), frameTime, scrollSpeeds,
		scales, distortion1, distortion2, distortion3, distortionScale, distortionBias))
	{
		return false;
	}

	// 알파 블렌딩을 끕니다.
	m_Direct3D->TurnOffAlphaBlending();

	// 렌더링 된 장면을 화면에 표시합니다.
	//m_Direct3D->EndScene();

	return true;
}

bool GraphicsClass::RenderBillboard()
{

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
	XMFLOAT3 cameraPosition, modelPosition;

	// 장면을 시작할 버퍼를 지운다.
	//m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 오쏘 (ortho) 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);


	translateMatrix = XMMatrixTranslation(5, -3, 0);
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

	// 드로잉을 준비하기 위해 바닥 파이프 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치합니다.
	m_FloorModel->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더를 사용하여 바닥 모델을 렌더링합니다.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FloorModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_FloorModel->GetTexture()))
	{
		return false;
	}

	// 카메라 위치를 얻는다.
	cameraPosition = m_Camera->GetPosition();

	// 빌보드 모델의 위치를 ​​설정합니다.
	modelPosition.x = 5.0f;
	modelPosition.y = -1.5f;
	modelPosition.z = 0.0f;

	// 아크 탄젠트 함수를 사용하여 현재 카메라 위치를 향하도록 빌보드 모델에 적용해야하는 회전을 계산합니다.
	double angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (180.0 / XM_PI);

	// 회전을 라디안으로 변환합니다.
	float rotation = (float)angle * 0.0174532925f;

	// 세계 행렬을 사용하여 원점에서 빌보드 회전을 설정합니다.
	worldMatrix = XMMatrixRotationY(rotation);



	// 빌보드 모델에서 번역 행렬을 설정합니다.
	translateMatrix = XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);

	// 마지막으로 회전 및 변환 행렬을 결합하여 빌보드 모델의 최종 행렬을 만듭니다.
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);


	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_BillboardModel->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 셰이더를 사용하여 모델을 렌더링합니다.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_BillboardModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_BillboardModel->GetTexture()))
	{
		return false;
	}

	// 렌더링 된 장면을 화면에 표시합니다.
	//m_Direct3D->EndScene();

	return true;
}

bool GraphicsClass::RenderDepth()
{
	// 카메라의 위치에 따라 뷰 행렬을 생성합니다
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);


	// 드로잉을 준비하기 위해 바닥 파이프 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치합니다.
	m_FloorModel->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더를 사용하여 바닥 모델을 렌더링합니다.
	if (!m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_FloorModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}


	worldMatrix = XMMatrixTranslation(0, 0, 10);
	
	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// 깊이 셰이더를 사용하여 모델을 렌더링합니다.
	if (!m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}

	return true;
}


bool GraphicsClass::RenderBlurSceneToTexture(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_BlurRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_BlurRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 회전 값으로 월드 행렬을 회전하여 큐브가 회전하도록합니다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_BlurModel->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 셰이더를 사용하여 모델을 렌더링합니다.
	if (!m_BlurTextureShader->Render(m_Direct3D->GetDeviceContext(), m_BlurModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_BlurModel->GetTexture()))
	{
		return false;
	}

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}


bool GraphicsClass::DownSampleTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_DownSampleTexure->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_DownSampleTexure->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 질감이 크기가 작기 때문에 렌더링에서 텍스처로 ortho 행렬을 가져옵니다.
	m_DownSampleTexure->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 드로잉을 준비하기 위해 그래픽 파이프 라인에 작은 ortho window 버텍스와 인덱스 버퍼를 놓습니다.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더를 사용하여 작은 ortho 창을 렌더링하고 씬의 텍스처를 텍스처 리소스로 렌더링합니다.
	if (!m_BlurTextureShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_BlurRenderTexture->GetShaderResourceView()))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}


bool GraphicsClass::RenderHorizontalBlurToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// 수평 블러 쉐이더에서 사용될 float에 화면 폭을 저장합니다.
	//float screenSizeX = (float)m_HorizontalBlurTexture->GetTextureWidth();

	float screenSizeX = (float)(SHADOWMAP_WIDTH / 2);
	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_HorizontalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_HorizontalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 텍스쳐가 다른 차원을 가지므로 렌더링에서 오쏘 (ortho) 행렬을 텍스처로 가져옵니다.
	m_HorizontalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 드로잉을 준비하기 위해 그래픽 파이프 라인에 작은 ortho window 버텍스와 인덱스 버퍼를 놓습니다.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// horizontal blur shader와 down sampled render를 사용하여 작은 ortho 윈도우를 텍스처 리소스로 렌더링합니다.	
	if (!m_HorizontalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_DownSampleTexure->GetShaderResourceView(), screenSizeX))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}


bool GraphicsClass::RenderVerticalBlurToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// 수직 블러 셰이더에서 사용되는 부동 소수점에 화면 높이를 저장합니다.
	//float screenSizeY = (float)m_VerticalBlurTexture->GetTextureHeight();

	float screenSizeY = (float)(SHADOWMAP_HEIGHT / 2);
	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_VerticalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_VerticalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 텍스쳐가 다른 차원을 가지므로 렌더링에서 오쏘 (ortho) 행렬을 텍스처로 가져옵니다.
	m_VerticalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 드로잉을 준비하기 위해 그래픽 파이프 라인에 작은 ortho window 버텍스와 인덱스 버퍼를 놓습니다.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// 수직 블러 쉐이더와 수평 블러 링을 사용하여 작은 ortho 윈도우를 텍스처 리소스로 렌더합니다.
	if (!m_VerticalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_HorizontalBlurTexture->GetShaderResourceView(), screenSizeY))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}


bool GraphicsClass::UpSampleTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_UpSampleTexure->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_UpSampleTexure->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 텍스쳐가 다른 차원을 가지므로 렌더링에서 오쏘 (ortho) 행렬을 텍스처로 가져옵니다.
	m_UpSampleTexure->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 그래픽 파이프 라인에 전체 화면 직교 윈도우 버텍스와 인덱스 버퍼를 배치하여 그리기를 준비합니다.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더와 텍스처 리소스에 대한 작은 크기의 최종 흐리게 렌더링을 사용하여 전체 화면 ortho 창을 렌더링합니다.
	if (!m_BlurTextureShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_VerticalBlurTexture->GetShaderResourceView()))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}


bool GraphicsClass::Render2DTextureScene()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// 장면을 시작할 버퍼를 지운다.
	//m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 오쏘 (ortho) 행렬을 가져옵니다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 그래픽 파이프 라인에 전체 화면 직교 윈도우 버텍스와 인덱스 버퍼를 배치하여 그리기를 준비합니다.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더를 사용하여 전체 화면 ortho 창을 렌더링하고 텍스처 리소스에 전체 화면 크기로 흐리게 렌더링합니다.
	if (!m_BlurTextureShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_UpSampleTexure->GetShaderResourceView()))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 된 장면을 화면에 표시합니다.
	//m_Direct3D->EndScene();

	return true;
}

bool GraphicsClass::RenderInstance()
{
	// 카메라의 위치에 따라 뷰 행렬을 생성합니다
	m_Camera->Render();
	static float movement = 0.01f;
	movement += 0.01f;
	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_InstanceModel->Render(m_Direct3D->GetDeviceContext());
	
	if (!m_InstanceModel->ChangeInstanceBuffer(m_Direct3D->GetDeviceContext(), movement)) return false;
	// 텍스쳐 쉐이더를 사용하여 모델을 렌더링합니다.
	if (!m_InstanceTextureShader->Render(m_Direct3D->GetDeviceContext(), m_InstanceModel->GetVertexCount(), m_InstanceModel->GetInstanceCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_InstanceModel->GetTexture()))
	{
		return false;
	}

	return true;
}

bool GraphicsClass::RenderTesellation()
{

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_TeselModel->Render(m_Direct3D->GetDeviceContext());

	// 테셀레이션 양
	float tessellationAmount = 20.0f;

	// 색상 쉐이더를 사용하여 모델을 렌더링합니다.
	if (!m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_TeselModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, tessellationAmount))
	{
		return false;
	}
	return true;
}

bool GraphicsClass::RenderParticle()
{
	// 카메라의 위치에 따라 뷰 행렬을 생성합니다
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 알파 블렌딩을 켭니다.
	m_Direct3D->TurnOnAlphaBlending();

	// 파티클 시스템 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	m_ParticleSystem->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 셰이더를 사용하여 모델을 렌더링합니다.
	if (!m_ShaderManager->RenderParticleShader(m_Direct3D->GetDeviceContext(), m_ParticleSystem->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_ParticleSystem->GetTexture()))
	{
		return false;
	}

	// 알파 블렌딩을 끕니다.
	m_Direct3D->TurnOffAlphaBlending();

	return true;
}

bool GraphicsClass::RenderShadowDepth()
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix;

	float posX = 0;
	float posY = 0;
	float posZ = 0;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_ShadowRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_ShadowRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 조명의 위치에 따라 조명보기 행렬을 생성합니다.
	m_ShadowLight->GenerateViewMatrix();

	// d3d 객체에서 세계 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 라이트 오브젝트에서 뷰 및 정사각형 매트릭스를 가져옵니다.
	m_ShadowLight->GetViewMatrix(lightViewMatrix);
	m_ShadowLight->GetProjectionMatrix(lightProjectionMatrix);

	// 큐브 모델에 대한 변환 행렬을 설정하십시오.
	m_ShadowCubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 큐브 모델을 렌더링합니다.
	m_ShadowCubeModel->Render(m_Direct3D->GetDeviceContext());
	bool result = m_ShadowDepthShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowCubeModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 구형 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowSphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 구형 모델을 렌더링합니다.
	m_ShadowSphereModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowDepthShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowSphereModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	
	// ground 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowGroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 그라운드 모델을 렌더링합니다.
	m_ShadowGroundModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowDepthShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowGroundModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}


bool GraphicsClass::RenderShadow()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;

	float posX = 0;
	float posY = 0;
	float posZ = 0;


	// 먼저 장면을 텍스처로 렌더링합니다.
	bool result = RenderShadowDepth();
	if (!result)
	{
		return false;
	}

	// 장면을 시작할 버퍼를 지운다.
	//m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 조명의 위치에 따라 조명보기 행렬을 생성합니다.
	m_ShadowLight->GenerateViewMatrix();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 라이트 오브젝트로부터 라이트의 뷰와 투영 행렬을 가져옵니다.
	m_ShadowLight->GetViewMatrix(lightViewMatrix);
	m_ShadowLight->GetProjectionMatrix(lightProjectionMatrix);


	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 구형 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowSphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_ShadowSphereModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowSphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_ShadowSphereModel->GetTexture(), m_ShadowRenderTexture->GetShaderResourceView(), m_ShadowLight->GetPosition(),
		m_ShadowLight->GetAmbientColor(), m_ShadowLight->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// 큐브 모델에 대한 변환 행렬을 설정하십시오.
	m_ShadowCubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 큐브 모델 정점과 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	m_ShadowCubeModel->Render(m_Direct3D->GetDeviceContext());

	// 그림자 쉐이더를 사용하여 모델을 렌더링합니다.
	result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowCubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_ShadowCubeModel->GetTexture(), m_ShadowRenderTexture->GetShaderResourceView(), m_ShadowLight->GetPosition(),
		m_ShadowLight->GetAmbientColor(), m_ShadowLight->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ground 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowGroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 그림자 쉐이더를 사용하여 그라운드 모델을 렌더링합니다.
	m_ShadowGroundModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowGroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_ShadowGroundModel->GetTexture(), m_ShadowRenderTexture->GetShaderResourceView(), m_ShadowLight->GetPosition(),
		m_ShadowLight->GetAmbientColor(), m_ShadowLight->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// 렌더링 된 장면을 화면에 표시합니다.
	//m_Direct3D->EndScene();

	return true;
}

bool GraphicsClass::RenderMultiShadowDepth()
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix;

	float posX = 0;
	float posY = 0;
	float posZ = 0;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_ShadowRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_ShadowRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 조명의 위치에 따라 조명보기 행렬을 생성합니다.
	m_ShadowLight->GenerateViewMatrix();

	// d3d 객체에서 세계 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 라이트 오브젝트에서 뷰 및 정사각형 매트릭스를 가져옵니다.
	m_ShadowLight->GetViewMatrix(lightViewMatrix);
	m_ShadowLight->GetProjectionMatrix(lightProjectionMatrix);



	// 구형 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowSphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 구형 모델을 렌더링합니다.
	m_ShadowSphereModel->Render(m_Direct3D->GetDeviceContext());
	bool result = m_ShadowDepthShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowSphereModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 큐브 모델에 대한 변환 행렬을 설정하십시오.
	m_ShadowCubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 큐브 모델을 렌더링합니다.
	m_ShadowCubeModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowDepthShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowCubeModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ground 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowGroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 그라운드 모델을 렌더링합니다.
	m_ShadowGroundModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowDepthShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowGroundModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderMultiShadowDepth2()
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix;

	float posX = 0;
	float posY = 0;
	float posZ = 0;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_ShadowRenderTexture2->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_ShadowRenderTexture2->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 조명의 위치에 따라 조명보기 행렬을 생성합니다.
	m_ShadowLight2->GenerateViewMatrix();

	// d3d 객체에서 세계 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 라이트 오브젝트에서 뷰 및 정사각형 매트릭스를 가져옵니다.
	m_ShadowLight2->GetViewMatrix(lightViewMatrix);
	m_ShadowLight2->GetProjectionMatrix(lightProjectionMatrix);



	// 구형 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowSphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 구형 모델을 렌더링합니다.
	m_ShadowSphereModel->Render(m_Direct3D->GetDeviceContext());
	bool result = m_ShadowDepthShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowSphereModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 큐브 모델에 대한 변환 행렬을 설정하십시오.
	m_ShadowCubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 큐브 모델을 렌더링합니다.
	m_ShadowCubeModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowDepthShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowCubeModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ground 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowGroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 그라운드 모델을 렌더링합니다.
	m_ShadowGroundModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowDepthShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowGroundModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderMultiShadow()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	XMMATRIX lightViewMatrix2, lightProjectionMatrix2;

	float posX = 0;
	float posY = 0;
	float posZ = 0;

	// 먼저 장면을 텍스처로 렌더링합니다.
	bool result = RenderMultiShadowDepth();
	if (!result)
	{
		return false;
	}
	// 먼저 장면을 텍스처로 렌더링합니다.
	result = RenderMultiShadowDepth2();
	if (!result)
	{
		return false;
	}

	// 장면을 시작할 버퍼를 지운다.
	//m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 조명의 위치에 따라 조명보기 행렬을 생성합니다.
	m_ShadowLight->GenerateViewMatrix();

	// 두 번째 빛에 대해서도 똑같이 설정합니다.
	m_ShadowLight2->GenerateViewMatrix();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 라이트 오브젝트로부터 라이트의 뷰와 투영 행렬을 가져옵니다.
	m_ShadowLight->GetViewMatrix(lightViewMatrix);
	m_ShadowLight->GetProjectionMatrix(lightProjectionMatrix);

	// 두 번째 빛에 대해서도 똑같이 설정합니다.
	m_ShadowLight2->GetViewMatrix(lightViewMatrix2);
	m_ShadowLight2->GetProjectionMatrix(lightProjectionMatrix2);

	// 큐브 모델에 대한 변환 행렬을 설정하십시오.
	m_ShadowCubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 큐브 모델 정점과 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	m_ShadowCubeModel->Render(m_Direct3D->GetDeviceContext());

	// 그림자 쉐이더를 사용하여 모델을 렌더링합니다.
	result = m_ShadowMultiShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowCubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_ShadowCubeModel->GetTexture(), m_ShadowRenderTexture->GetShaderResourceView(), m_ShadowLight->GetPosition(),
		m_ShadowLight->GetAmbientColor(), m_ShadowLight->GetDiffuseColor(), lightViewMatrix2, lightProjectionMatrix2,
		m_ShadowRenderTexture2->GetShaderResourceView(), m_ShadowLight2->GetPosition(), m_ShadowLight2->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 구형 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowSphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_ShadowSphereModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowMultiShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowSphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_ShadowSphereModel->GetTexture(), m_ShadowRenderTexture->GetShaderResourceView(), m_ShadowLight->GetPosition(),
		m_ShadowLight->GetAmbientColor(), m_ShadowLight->GetDiffuseColor(), lightViewMatrix2, lightProjectionMatrix2,
		m_ShadowRenderTexture2->GetShaderResourceView(), m_ShadowLight2->GetPosition(), m_ShadowLight2->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ground 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowGroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 그림자 쉐이더를 사용하여 그라운드 모델을 렌더링합니다.
	m_ShadowGroundModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowMultiShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowGroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_ShadowGroundModel->GetTexture(), m_ShadowRenderTexture->GetShaderResourceView(), m_ShadowLight->GetPosition(),
		m_ShadowLight->GetAmbientColor(), m_ShadowLight->GetDiffuseColor(), lightViewMatrix2, lightProjectionMatrix2,
		m_ShadowRenderTexture2->GetShaderResourceView(), m_ShadowLight2->GetPosition(), m_ShadowLight2->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// 렌더링 된 장면을 화면에 표시합니다.
	//m_Direct3D->EndScene();

	return true;
}

bool GraphicsClass::RenderMultiShadow2()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	XMMATRIX lightViewMatrix2, lightProjectionMatrix2;
	XMMATRIX lightViewMatrix3, lightOrthoMatrix;

	float posX = 0;
	float posY = 0;
	float posZ = 0;

	// 먼저 장면을 텍스처로 렌더링합니다.
	bool result = RenderMultiShadowDepth();
	if (!result)
	{
		return false;
	}
	// 먼저 장면을 텍스처로 렌더링합니다.
	result = RenderMultiShadowDepth2();
	if (!result)
	{
		return false;
	}
	// 먼저 장면을 텍스처로 렌더링합니다.
	result = RenderMultiShadowDepth3();
	if (!result)
	{
		return false;
	}

	// 장면을 시작할 버퍼를 지운다.
	//m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 조명의 위치에 따라 조명보기 행렬을 생성합니다.
	m_ShadowLight->GenerateViewMatrix();

	// 두 번째 빛에 대해서도 똑같이 설정합니다.
	m_ShadowLight2->GenerateViewMatrix();

	// 세 번째 빛에 대해서도 똑같이 설정합니다.
	m_ShadowLight3->GenerateViewMatrix();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 라이트 오브젝트로부터 라이트의 뷰와 투영 행렬을 가져옵니다.
	m_ShadowLight->GetViewMatrix(lightViewMatrix);
	m_ShadowLight->GetProjectionMatrix(lightProjectionMatrix);

	// 두 번째 빛에 대해서도 똑같이 설정합니다.
	m_ShadowLight2->GetViewMatrix(lightViewMatrix2);
	m_ShadowLight2->GetProjectionMatrix(lightProjectionMatrix2);

	// 라이트 오브젝트로부터 라이트의 뷰와 투영 행렬을 가져옵니다.
	m_ShadowLight3->GetViewMatrix(lightViewMatrix3);
	m_ShadowLight3->GetOrthoMatrix(lightOrthoMatrix);

	// 큐브 모델에 대한 변환 행렬을 설정하십시오.
	m_ShadowCubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 큐브 모델 정점과 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	m_ShadowCubeModel->Render(m_Direct3D->GetDeviceContext());

	// 그림자 쉐이더를 사용하여 모델을 렌더링합니다.
	result = m_ShadowMultiShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowCubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_ShadowCubeModel->GetTexture(), m_ShadowRenderTexture->GetShaderResourceView(), m_ShadowLight->GetPosition(),
		m_ShadowLight->GetAmbientColor(), m_ShadowLight->GetDiffuseColor(), lightViewMatrix2, lightProjectionMatrix2,
		m_ShadowRenderTexture2->GetShaderResourceView(), m_ShadowLight2->GetPosition(), m_ShadowLight2->GetDiffuseColor(),
		lightViewMatrix3,	lightOrthoMatrix, m_ShadowRenderTexture3->GetShaderResourceView(), m_ShadowLight3->GetDirection(),
		m_ShadowLight3->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 구형 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowSphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_ShadowSphereModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowMultiShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowSphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_ShadowSphereModel->GetTexture(), m_ShadowRenderTexture->GetShaderResourceView(), m_ShadowLight->GetPosition(),
		m_ShadowLight->GetAmbientColor(), m_ShadowLight->GetDiffuseColor(), lightViewMatrix2, lightProjectionMatrix2,
		m_ShadowRenderTexture2->GetShaderResourceView(), m_ShadowLight2->GetPosition(), m_ShadowLight2->GetDiffuseColor(),
		lightViewMatrix3, lightOrthoMatrix, m_ShadowRenderTexture3->GetShaderResourceView(), m_ShadowLight3->GetDirection(),
		m_ShadowLight3->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ground 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowGroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 그림자 쉐이더를 사용하여 그라운드 모델을 렌더링합니다.
	m_ShadowGroundModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowMultiShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowGroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_ShadowGroundModel->GetTexture(), m_ShadowRenderTexture->GetShaderResourceView(), m_ShadowLight->GetPosition(),
		m_ShadowLight->GetAmbientColor(), m_ShadowLight->GetDiffuseColor(), lightViewMatrix2, lightProjectionMatrix2,
		m_ShadowRenderTexture2->GetShaderResourceView(), m_ShadowLight2->GetPosition(), m_ShadowLight2->GetDiffuseColor(),
		lightViewMatrix3, lightOrthoMatrix, m_ShadowRenderTexture3->GetShaderResourceView(), m_ShadowLight3->GetDirection(),
		m_ShadowLight3->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// 렌더링 된 장면을 화면에 표시합니다.
	//m_Direct3D->EndScene();

	return true;
}

bool GraphicsClass::SoftShadowDownSampleTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_DownSampleTexure->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_DownSampleTexure->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다.
	//m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);

	// 질감이 크기가 작기 때문에 렌더링에서 텍스처로 ortho 행렬을 가져옵니다.
	m_DownSampleTexure->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 드로잉을 준비하기 위해 그래픽 파이프 라인에 작은 ortho window 버텍스와 인덱스 버퍼를 놓습니다.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더를 사용하여 작은 ortho 창을 렌더링하고 씬의 텍스처를 텍스처 리소스로 렌더링합니다.
	if (!m_BlurTextureShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_BlackWhiteRenderTexture->GetShaderResourceView()))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderBlackAndWhiteShadows()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	float posX, posY, posZ;
	bool result;


	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_BlackWhiteRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_BlackWhiteRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 조명의 위치에 따라 조명보기 행렬을 생성합니다.
	m_ShadowLight->GenerateViewMatrix();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다. 
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 라이트 오브젝트로부터 라이트의 뷰와 투영 행렬을 가져옵니다. 
	m_ShadowLight->GetViewMatrix(lightViewMatrix);
	m_ShadowLight->GetProjectionMatrix(lightProjectionMatrix);

	// 큐브 모델에 대한 변환 행렬을 설정하십시오. 
	m_ShadowCubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 그림자 쉐이더를 사용하여 큐브 모델을 렌더링합니다. 
	m_ShadowCubeModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowBlackWhiteShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowCubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_ShadowRenderTexture->GetShaderResourceView(), m_ShadowLight->GetPosition());
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다. 
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 구형 모델에 대한 변환 행렬을 설정합니다. 
	m_ShadowSphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 그림자 쉐이더를 사용하여 구형 모델을 렌더링합니다. 
	m_ShadowSphereModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowBlackWhiteShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowSphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_ShadowRenderTexture->GetShaderResourceView(), m_ShadowLight->GetPosition());
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다. 
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ground 모델에 대한 변환 행렬을 설정합니다. 
	m_ShadowGroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 그림자 쉐이더를 사용하여 그라운드 모델을 렌더링합니다. 
	m_ShadowGroundModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowBlackWhiteShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowGroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_ShadowRenderTexture->GetShaderResourceView(), m_ShadowLight->GetPosition());
	if (!result)
	{
		return false;
	}

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링 대상을 더 이상 텍스처로 다시 설정하지 않습니다. 
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다. 
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::SoftShadowRenderHorizontalBlurToTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	float screenSizeX;
	bool result;


	// 수평 블러 쉐이더에서 사용될 float에 화면 폭을 저장합니다. 
	screenSizeX = (float)(SHADOWMAP_WIDTH / 2);

	// 렌더링 대상을 렌더링에 맞게 설정합니다. 
	m_HorizontalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다. 
	m_HorizontalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다. 
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다.
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 텍스쳐가 다른 차원을 가지므로 렌더링에서 오쏘 (ortho) 행렬을 텍스처로 가져옵니다. 
	m_HorizontalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다. 
	m_Direct3D->TurnZBufferOff();

	// 드로잉을 준비하기 위해 그래픽 파이프 라인에 작은 ortho window 버텍스와 인덱스 버퍼를 놓습니다. 
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// horizontal blur shader와 down sampled render를 사용하여 작은 ortho 윈도우를 텍스처 리소스로 렌더링합니다. 
	result = m_HorizontalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_DownSampleTexure->GetShaderResourceView(), screenSizeX);
	if (!result)
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다. 
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다. 
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::SoftShadowRenderVerticalBlurToTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	float screenSizeY;
	bool result;


	// 수직 블러 셰이더에서 사용되는 부동 소수점에 화면 높이를 저장합니다. 
	screenSizeY = (float)(SHADOWMAP_HEIGHT / 2);

	// 렌더링 대상을 렌더링에 맞게 설정합니다. 
	m_VerticalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다. 
	m_VerticalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다. 
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다.
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 텍스쳐가 다른 차원을 가지므로 렌더링에서 오쏘 (ortho) 행렬을 텍스처로 가져옵니다. 
	m_VerticalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다. 
	m_Direct3D->TurnZBufferOff();

	// 드로잉을 준비하기 위해 그래픽 파이프 라인에 작은 ortho window 버텍스와 인덱스 버퍼를 놓습니다. 
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// 수직 블러 쉐이더와 수평 블러 링을 사용하여 작은 ortho 윈도우를 텍스처 리소스로 렌더합니다. 
	result = m_VerticalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_HorizontalBlurTexture->GetShaderResourceView(), screenSizeY);
	if (!result)
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오. 
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다. 
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다. 
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::SoftShadowUpSampleTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	bool result;


	// 렌더링 대상을 렌더링에 맞게 설정합니다. 
	m_UpSampleTexure->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다. 
	m_UpSampleTexure->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다. 
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다. 
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 텍스쳐가 다른 차원을 가지므로 렌더링에서 오쏘 (ortho) 행렬을 텍스처로 가져옵니다.
	m_UpSampleTexure->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다. 
	m_Direct3D->TurnZBufferOff();

	// 그래픽 파이프 라인에 전체 화면 직교 윈도우 버텍스와 인덱스 버퍼를 배치하여 그리기를 준비합니다. 
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더와 텍스처 리소스에 대한 작은 크기의 최종 흐리게 렌더링을 사용하여 전체 화면 ortho 창을 렌더링합니다. 
	result = m_BlurTextureShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_VerticalBlurTexture->GetShaderResourceView());
	if (!result)
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오. 
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다. 
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다. 
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderSoftShadow()
{
	bool result;
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	float posX, posY, posZ;


	// 먼저 장면을 텍스처로 렌더링합니다. 
	result = RenderShadowDepth();
	if (!result)
	{
		return false;
	}

	// 다음으로 그림자가있는 장면을 흑백으로 렌더링합니다. 
	result = RenderBlackAndWhiteShadows();
	if (!result)
	{
		return false;
	}

	// 그런 다음 흑백 씬 텍스처를 샘플링합니다. 
	result = SoftShadowDownSampleTexture();
	if (!result)
	{
		return false;
	}

	// 다운 샘플링 된 텍스처에 수평 블러를 수행합니다. 
	result = SoftShadowRenderHorizontalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// 이제 텍스처에 수직 블러를 수행합니다. 
	result = SoftShadowRenderVerticalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// 최종적으로 부드러운 그림자 쉐이더에서 사용할 수있는 텍스처에 대한 최종 흐린 렌더링 샘플을 샘플링합니다. 
	result = SoftShadowUpSampleTexture();
	if (!result)
	{
		return false;
	}

	// 버퍼를 지우고 장면을 시작합니다. 
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 조명의 위치에 따라 조명보기 행렬을 생성합니다.
	m_ShadowLight->GenerateViewMatrix();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 큐브 모델에 대한 변환 행렬을 설정하십시오. 
	m_ShadowCubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 부드러운 그림자 셰이더를 사용하여 큐브 모델을 렌더링합니다. 
	m_ShadowCubeModel->Render(m_Direct3D->GetDeviceContext());
	result = m_SoftShadowShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowCubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_ShadowCubeModel->GetTexture(), m_UpSampleTexure->GetShaderResourceView(), m_ShadowLight->GetPosition(),
		m_ShadowLight->GetAmbientColor(), m_ShadowLight->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 구형 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowSphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 부드러운 그림자 쉐이더를 사용하여 구형 모델을 렌더링합니다. 
	m_ShadowSphereModel->Render(m_Direct3D->GetDeviceContext());
	result = m_SoftShadowShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowSphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_ShadowSphereModel->GetTexture(), m_UpSampleTexure->GetShaderResourceView(), m_ShadowLight->GetPosition(),
		m_ShadowLight->GetAmbientColor(), m_ShadowLight->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ground 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowGroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 부드러운 그림자 쉐이더를 사용하여 그라운드 모델을 렌더링합니다. 
	m_ShadowGroundModel->Render(m_Direct3D->GetDeviceContext());
	result = m_SoftShadowShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowGroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_ShadowGroundModel->GetTexture(), m_UpSampleTexure->GetShaderResourceView(), m_ShadowLight->GetPosition(),
		m_ShadowLight->GetAmbientColor(), m_ShadowLight->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// 렌더링 된 장면을 화면에 표시합니다.
	//m_Direct3D->EndScene();

	return true;
}

bool GraphicsClass::RenderProjection()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX viewMatrix2, projectionMatrix2;

	// 씬을 그리기 위해 버퍼를 지웁니다
	//m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 뷰 포인트 객체에서 뷰 및 투영 행렬을 가져옵니다.
	static float rotationview = 0.0f;
	rotationview += 0.01f;
	XMMATRIX rotationmatrix;
	rotationmatrix = XMMatrixRotationY(rotationview);
	m_ViewPoint->GetViewMatrix(viewMatrix2);
	m_ViewPoint->GetProjectionMatrix(projectionMatrix2);
	viewMatrix2 = XMMatrixMultiply(viewMatrix2, rotationmatrix);

	// ground 모델에 대한 번역을 설정합니다.
	worldMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);

	// 투영 셰이더를 사용하여 지상 모델을 렌더링합니다.
	m_ProGroundModel->Render(m_Direct3D->GetDeviceContext());

	if (!m_ShaderManager->RenderProjectionShader(m_Direct3D->GetDeviceContext(), m_ProGroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_ProGroundModel->GetTexture(), m_ProLight->GetAmbientColor(), m_ProLight->GetDiffuseColor(), m_ProLight->GetDirection(),
		viewMatrix2, projectionMatrix2, m_ProjectionTexture->GetTexture()))
	{
		return false;
	}

	// 월드 행렬을 재설정하고 큐브 모델에 대한 변환을 설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);

	// 프로젝션 셰이더를 사용하여 큐브 모델을 렌더링합니다.
	m_ProCubeModel->Render(m_Direct3D->GetDeviceContext());

	if (!m_ShaderManager->RenderProjectionShader(m_Direct3D->GetDeviceContext(), m_ProCubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_ProCubeModel->GetTexture(), m_ProLight->GetAmbientColor(), m_ProLight->GetDiffuseColor(), m_ProLight->GetDirection(),
		viewMatrix2, projectionMatrix2, m_ProjectionTexture->GetTexture()))
	{
		return false;
	}

	// 렌더링 된 장면을 화면에 표시합니다.
	//m_Direct3D->EndScene();

	return true;
}

bool GraphicsClass::RenderGlow()
{
	// 글로우 맵을 렌더링 텍스처에 먼저 렌더링합니다.
	bool result = RenderGlowMapToTexture();
	if (!result)
	{
		return false;
	}

	// 다음으로 렌더 텍스처를 작은 크기의 텍스처로 샘플링합니다.
	result = GlowDownSampleTexture();
	if (!result)
	{
		return false;
	}

	// 다운 샘플링 된 렌더링 텍스처에 수평 블러를 수행합니다.
	result = GlowRenderHorizontalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// 이제 수평 흐림 렌더 텍스처에서 수직 흐림 효과를 수행합니다.
	result = GlowRenderVerticalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// 최종 흐린 렌더링 텍스처를 화면 크기로 다시 샘플링합니다.
	result = GlowUpSampleTexture();
	if (!result)
	{
		return false;
	}

	// 일반 UI 요소를 전체 화면 텍스처로 렌더링합니다.
	//result = RenderUIElementsToTexture();
	//if (!result)
	//{
	//	return false;
	//}
	RenderUIElementsToTexture();
	// UI 요소와 빛나는 UI 요소를 결합한 최종 장면을 렌더링합니다.
	RenderGlowScene();

	return true;
}

bool GraphicsClass::RenderGlowMapToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_GlowRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_GlowRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 비트 맵 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	if (!m_GlowBitmap->Render(m_Direct3D->GetDeviceContext(), 100, 100))
	{
		return false;
	}

	// 글로우 맵 셰이더를 사용하여 비트 맵을 렌더링합니다.
	m_ShaderManager->RenderGlowMapShader(m_Direct3D->GetDeviceContext(), m_GlowBitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_GlowBitmap->GetTexture(), m_GlowBitmap->GetGlowMap());

	//모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderUIElementsToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;


	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_GlowRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_GlowRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 오쏘 (ortho) 행렬을 가져옵니다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 비트 맵 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	result = m_GlowBitmap->Render(m_Direct3D->GetDeviceContext(), 100, 100);
	if (!result)
	{
		return false;
	}

	// 텍스처 쉐이더를 사용하여 비트 맵을 렌더링합니다.
	result = m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_GlowBitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_GlowBitmap->GetTexture());
	if (!result)
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderGlowScene()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// 장면을 시작할 버퍼를 지운다.
	//m_Direct3D->BeginScene(1.0f, 0.0f, 0.0f, 0.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 오쏘 (ortho) 행렬을 가져옵니다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 그래픽 파이프 라인에 전체 화면 직교 윈도우 버텍스와 인덱스 버퍼를 배치하여 그리기를 준비합니다.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더를 사용하여 전체 화면 ortho 창을 렌더링하고 텍스처 리소스에 전체 화면 크기로 흐리게 렌더링합니다.
	m_ShaderManager->RenderGlowShader(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_GlowRenderTexture->GetShaderResourceView(), m_UpSampleTexure->GetShaderResourceView(), 3.0f);

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 된 장면을 화면에 표시합니다.
	//m_Direct3D->EndScene();

	return true;
}

bool GraphicsClass::GlowDownSampleTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_DownSampleTexure->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_DownSampleTexure->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 질감이 크기가 작기 때문에 렌더링에서 텍스처로 ortho 행렬을 가져옵니다.
	m_DownSampleTexure->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 드로잉을 준비하기 위해 그래픽 파이프 라인에 작은 ortho window 버텍스와 인덱스 버퍼를 놓습니다.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더를 사용하여 작은 ortho 창을 렌더링하고 씬의 텍스처를 텍스처 리소스로 렌더링합니다.
	if (!m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_GlowRenderTexture->GetShaderResourceView()))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::GlowRenderHorizontalBlurToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_HorizontalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_HorizontalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 텍스쳐가 다른 차원을 가지므로 렌더링에서 오쏘 (ortho) 행렬을 텍스처로 가져옵니다.
	m_HorizontalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 수평 블러 쉐이더에서 사용될 float에 화면 폭을 저장합니다.
	float screenSizeX = (float)m_HorizontalBlurTexture->GetTextureWidth();

	// 드로잉을 준비하기 위해 그래픽 파이프 라인에 작은 ortho window 버텍스와 인덱스 버퍼를 놓습니다.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// horizontal blur shader와 down sampled render를 사용하여 작은 ortho 윈도우를 텍스처 리소스로 렌더링합니다.	
	if (!m_ShaderManager->RenderHorizontalBlurShader(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_DownSampleTexure->GetShaderResourceView(), screenSizeX))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::GlowRenderVerticalBlurToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_VerticalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_VerticalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 텍스쳐가 다른 차원을 가지므로 렌더링에서 오쏘 (ortho) 행렬을 텍스처로 가져옵니다.
	m_VerticalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 수직 블러 셰이더에서 사용되는 부동 소수점에 화면 높이를 저장합니다.
	float screenSizeY = (float)m_VerticalBlurTexture->GetTextureHeight();

	// 드로잉을 준비하기 위해 그래픽 파이프 라인에 작은 ortho window 버텍스와 인덱스 버퍼를 놓습니다.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// 수직 블러 쉐이더와 수평 블러 링을 사용하여 작은 ortho 윈도우를 텍스처 리소스로 렌더합니다.
	if (!m_ShaderManager->RenderVerticalBlurShader(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_HorizontalBlurTexture->GetShaderResourceView(), screenSizeY))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::GlowUpSampleTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_UpSampleTexure->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_UpSampleTexure->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 텍스쳐가 다른 차원을 가지므로 렌더링에서 오쏘 (ortho) 행렬을 텍스처로 가져옵니다.
	m_UpSampleTexure->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 그래픽 파이프 라인에 전체 화면 직교 윈도우 버텍스와 인덱스 버퍼를 배치하여 그리기를 준비합니다.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더와 텍스처 리소스에 대한 작은 크기의 최종 흐리게 렌더링을 사용하여 전체 화면 ortho 창을 렌더링합니다.
	if (!m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_VerticalBlurTexture->GetShaderResourceView()))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderMultiShadowDepth3()
{
	XMMATRIX worldMatrix, lightViewMatrix, lightOrthoMatrix;

	float posX = 0;
	float posY = 0;
	float posZ = 0;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_ShadowRenderTexture3->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_ShadowRenderTexture3->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 조명의 위치에 따라 조명보기 행렬을 생성합니다.
	m_ShadowLight3->GenerateViewMatrix();

	// d3d 객체에서 세계 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 라이트 오브젝트에서 뷰 및 정사각형 매트릭스를 가져옵니다.
	m_ShadowLight3->GetViewMatrix(lightViewMatrix);
	m_ShadowLight3->GetOrthoMatrix(lightOrthoMatrix);

	// 구형 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowSphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 구형 모델을 렌더링합니다.
	m_ShadowSphereModel->Render(m_Direct3D->GetDeviceContext());
	bool result = m_ShadowDepthShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowSphereModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 큐브 모델에 대한 변환 행렬을 설정하십시오.
	m_ShadowCubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 큐브 모델을 렌더링합니다.
	m_ShadowCubeModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowDepthShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowCubeModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ground 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowGroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 그라운드 모델을 렌더링합니다.
	m_ShadowGroundModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowDepthShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowGroundModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);
	if (!result)
	{
		return false;
	}

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderTransparentShadowDepth()
{
	XMMATRIX worldMatrix, lightViewMatrix, lightOrthoMatrix;
	XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// 렌더링 대상을 텍스처에 렌더링으로 설정합니다.
	m_TransparentDepthRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링에 텍스처를 지웁니다.
	m_TransparentDepthRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// d3d 객체에서 월드 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 조명의 위치에 따라 조명보기 행렬을 생성합니다.
	m_ShadowLight->GenerateViewMatrix();

	// 라이트 오브젝트에서 뷰 및 정사각형 매트릭스를 가져옵니다.
	m_ShadowLight->GetViewMatrix(lightViewMatrix);
	m_ShadowLight->GetOrthoMatrix(lightOrthoMatrix);

	// 나무의 위치로 변환합니다.
	m_Tree->GetPosition(pos);
	worldMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);

	// 깊이 쉐이더로 나무 트렁크를 렌더링합니다.
	m_Tree->RenderTrunk(m_Direct3D->GetDeviceContext());
	m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Tree->GetTrunkIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);

	// 깊이 투명도 셰이더를 사용하여 나무와 나뭇잎을 렌더링 합니다.
	m_Tree->RenderLeaves(m_Direct3D->GetDeviceContext());
	if (!m_ShaderManager->RenderTransparentDepthShader(m_Direct3D->GetDeviceContext(), m_Tree->GetLeafIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix, m_Tree->GetLeafTexture()))
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ground 모델에 대한 변환 행렬을 설정합니다.
	m_ShadowGroundModel->GetPosition(pos);
	worldMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);

	// 그림자 쉐이더를 사용하여 그라운드 모델을 렌더링합니다.
	m_ShadowGroundModel->Render(m_Direct3D->GetDeviceContext());
	m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_ShadowGroundModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderTransparentShadow()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightOrthoMatrix;
	XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// 먼저 장면을 텍스처로 렌더링합니다.
	if (!RenderTransparentShadowDepth())
	{
		return false;
	}

	// 장면을 시작할 버퍼를 지운다.
	//m_Direct3D->BeginScene(0.0f, 0.5f, 0.8f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 조명의 위치에 따라 조명보기 행렬을 생성합니다.
	m_ShadowLight->GenerateViewMatrix();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 라이트 오브젝트로부터 라이트의 뷰와 투영 행렬을 가져옵니다.
	m_ShadowLight->GetViewMatrix(lightViewMatrix);
	m_ShadowLight->GetOrthoMatrix(lightOrthoMatrix);

	// 밝은 색상 속성을 설정합니다.
	XMFLOAT4 diffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 ambientColor = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	// 지상 모델의 위치로 변환합니다.
	m_ShadowGroundModel->GetPosition(pos);
	worldMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);

	// 그림자 쉐이더를 사용하여 그라운드 모델을 렌더링 합니다.
	m_ShadowGroundModel->Render(m_Direct3D->GetDeviceContext());
	m_ShaderManager->RenderShadowDirectionalShader(m_Direct3D->GetDeviceContext(), m_ShadowGroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightOrthoMatrix, m_ShadowGroundModel->GetTexture(), m_TransparentDepthRenderTexture->GetShaderResourceView(), m_ShadowLight->GetDirection(),
		ambientColor, diffuseColor);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 트리 모델의 위치로 변환합니다.
	m_Tree->GetPosition(pos);
	worldMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);

	// 나무 줄기를 렌더링 합니다.
	m_Tree->RenderTrunk(m_Direct3D->GetDeviceContext());
	m_ShaderManager->RenderShadowDirectionalShader(m_Direct3D->GetDeviceContext(), m_Tree->GetTrunkIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightOrthoMatrix, m_Tree->GetTrunkTexture(), m_TransparentDepthRenderTexture->GetShaderResourceView(), m_ShadowLight->GetDirection(),
		ambientColor, diffuseColor);

	// 블렌딩을 활성화하고 나무를 렌더링 합니다.
	m_Direct3D->TurnOnAlphaBlending();
	m_Tree->RenderLeaves(m_Direct3D->GetDeviceContext());
	m_ShaderManager->RenderShadowDirectionalShader(m_Direct3D->GetDeviceContext(), m_Tree->GetLeafIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightOrthoMatrix, m_Tree->GetLeafTexture(), m_TransparentDepthRenderTexture->GetShaderResourceView(), m_ShadowLight->GetDirection(),
		ambientColor, diffuseColor);
	m_Direct3D->TurnOffAlphaBlending();
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 렌더링 된 장면을 화면에 출력합니다.
	//m_Direct3D->EndScene();

	return true;
}

bool GraphicsClass::RenderDeferredTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// 렌더링 대상을 텍스처에 렌더링으로 설정합니다.
	m_DeferredBuffers->SetRenderTargets(m_Direct3D->GetDeviceContext());

	// 렌더링에 텍스처를 지웁니다.
	m_DeferredBuffers->ClearRenderTargets(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// d3d 객체에서 월드 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 각 프레임의 rotation 변수를 업데이트합니다.
	static float rotation = 0.0f;
	rotation += (float)XM_PI * 0.0007f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// 회전값으로 월드 행렬을 회전하여 큐브가 회전하도록 합니다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_BlurModel->Render(m_Direct3D->GetDeviceContext());

	// 지연된 셰이더를 사용하여 모델을 렌더링합니다.
	m_ShaderManager->RenderDeferredShader(m_Direct3D->GetDeviceContext(), m_BlurModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_BlurModel->GetTexture());


	static float frameTime = 0.0f;


	// 프레임 시간 카운터를 증가시킵니다.
	frameTime += 0.01f;
	if (frameTime > 1000.0f)
	{
		frameTime = 0.0f;
	}

	// 세 가지 다른 노이즈 텍스처에 대해 세 가지 스크롤 속도를 설정합니다.
	XMFLOAT3 scrollSpeeds = XMFLOAT3(1.3f, 2.1f, 2.3f);

	// 세 개의 서로 다른 노이즈 옥타브 텍스처를 만드는 데 사용할 세 개의 스케일을 설정합니다.
	XMFLOAT3 scales = XMFLOAT3(1.0f, 2.0f, 3.0f);

	// 세 가지 다른 노이즈 텍스처에 대해 세 가지 다른 x 및 y 왜곡 인수를 설정합니다.
	XMFLOAT2 distortion1 = XMFLOAT2(0.1f, 0.2f);
	XMFLOAT2 distortion2 = XMFLOAT2(0.1f, 0.3f);
	XMFLOAT2 distortion3 = XMFLOAT2(0.1f, 0.1f);

	// 텍스처 좌표 샘플링 섭동의 스케일과 바이어스.
	float distortionScale = 0.8f;
	float distortionBias = 0.5f;

	// 장면을 시작할 버퍼를 지운다.
	//m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 오쏘 (ortho) 행렬을 가져옵니다.
	//XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	XMMATRIX trans = XMMatrixTranslation(g_data->firePosition[0], g_data->firePosition[1], g_data->firePosition[2]);
	worldMatrix = XMMatrixMultiply(worldMatrix, trans);
	// 화재 투명도의 알파 블렌드를 켭니다.
	m_Direct3D->TurnOnAlphaBlending();

	// 정사각형 모델의 정점과 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	m_ModelFire->Render(m_Direct3D->GetDeviceContext());

	// 화재 쉐이더를 사용하여 사각형 모델을 렌더링합니다.
	if (!m_FireShader->Render(m_Direct3D->GetDeviceContext(), m_ModelFire->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_ModelFire->GetTexture(1), m_ModelFire->GetTexture(2), m_ModelFire->GetTexture(3), frameTime, scrollSpeeds,
		scales, distortion1, distortion2, distortion3, distortionScale, distortionBias))
	{
		return false;
	}

	// 알파 블렌딩을 끕니다.
	m_Direct3D->TurnOffAlphaBlending();


	// 렌더 버퍼가 아닌 원래의 백 버퍼로 렌더링 타겟을 다시 설정합니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderDeferred()
{
	XMMATRIX worldMatrix, baseViewMatrix, viewMatrix,  orthoMatrix, projectionMatrix;

	// 먼저 장면을 텍스처로 렌더링합니다.
	if (!RenderDeferredTexture())
	{
		return false;
	}

	// 장면을 시작할 버퍼를 지운다.
	//m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 그래픽 파이프 라인에 전체 화면 직교 윈도우 버텍스와 인덱스 버퍼를 배치하여 그리기를 준비합니다.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더와 텍스처 리소스에 대한 작은 크기의 최종 흐리게 렌더링을 사용하여 전체 화면 ortho 창을 렌더링합니다.
	//if (!m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
	//	m_DeferredBuffers->GetShaderResourceView(0)))

	//// 지연 조명 쉐이더와 렌더링 버퍼를 사용하여 전체 화면 정사영 윈도우를 렌더링합니다.
	m_ShaderManager->RenderDeferredLightShader(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_DeferredBuffers->GetShaderResourceView(0), m_DeferredBuffers->GetShaderResourceView(1),
		m_ProLight->GetDirection());

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켭니다.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 된 장면을 화면에 표시합니다.
	//m_Direct3D->EndScene();

	return true;
}

