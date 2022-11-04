#include "stdafx.h"
#include "d3dclass.h"
#include "Utils/Imgui/imgui.h"
#include "Utils/Imgui/imgui_impl_dx11.h"
#include "Utils/Imgui/imgui_impl_win32.h"
#include "Systems/Application/Application.h"
#include "Graphics.h"
#include "ShaderClass/shadermanagerclass.h"
#include "RenderTexture.h"
#include "orthowindowclass.h"
#include "ShadowRenderTextures.h"

#include "DeferredBuffersClass.h"
#include "Systems/Application/Application.h"
#include "Component/ComponentHeaders.h"
#include "Object/ObjectClass.h"
#include "Systems/Input/inputClass.h"

#include <string>

Graphics* mGRAPHICS = nullptr;

Graphics::Graphics()
{
	assert(mGRAPHICS == nullptr && "No more than one instance of Application system!");
	mGRAPHICS = this;
}

Graphics::~Graphics()
{
}

void Graphics::Initialize()
{
	int screenWidth = mAPPLICATION->GetWidth();
	int screenHeight = mAPPLICATION->GetHeight();
	// Set down sampling size
	int downSampleWidth = screenWidth / 2;
	int downSampleHeight = screenHeight / 2;
	bool result = true;

	//Create D3D object
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return ;
	}

	//initialize d3d
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, mAPPLICATION->GetHWND(),
		FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(mAPPLICATION->GetHWND(), L"Direct3D 객체생성 오류", L"Error", MB_OK);
		return;
	}

	// Create Shader Manager
	m_ShaderManager = new ShaderManagerClass;
	if (!m_ShaderManager)
	{
		return ;
	}

	// Shader manaeger initialize
	if (!m_ShaderManager->Initialize(m_Direct3D->GetDevice(), mAPPLICATION->GetHWND()))
	{
		MessageBox(mAPPLICATION->GetHWND(), L"fail initialize the color shader object.", L"Error", MB_OK);
		return ;
	}


	// Create & Initialize full ortho window
	m_FullScreenWindow = new OrthoWindowClass;
	m_FullScreenWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight);


	// Create & initializedeferred buffer
	m_DeferredBuffers = new DeferredBuffersClass;
	m_DeferredBuffers->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);

	// Initialize Shadow render texture
	m_ShadowTexture = new ShadowRenderTextures();
	m_ShadowTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);


	//Create Imgui data
	g_data = new GraphicsDatas;


	// Setup Dear ImGui context
	ImguiInitialize();

	m_CameraNumber = 0;
}

void Graphics::Update(float dt)
{
	bool result = false;

	// Clear Begin Back color
	ImguiUpdate(dt);

	//Here render all
	MakeShadow();
	RenderDeferredToTexture();

	m_Direct3D->BeginScene(g_data->clear_color[0], g_data->clear_color[1], g_data->clear_color[2], 1.0f);
	m_Camera[m_CameraNumber]->Render();

	
	switch (m_renderlist)
	{
		case RenderList::NORMAL:
		{
			RenderNormal();
			break;
		}
		case RenderList::DEFERRED:
		{
			RenderDeferred();
			break;
		}
		case RenderList::RESOURCES:
		{
			RenderResources();
			break;
		}
		case RenderList::SHADOWS:
		{
			RenderShadows();
			break;
		}
	}

	//Imgui Render
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Swap chain
	m_Direct3D->EndScene();

}

void Graphics::Shutdown()
{
	// Cleanup
	ImguiShutdown();

	//Release buffer / objects
	if (m_ShadowTexture)
	{
		m_ShadowTexture->Shutdown();
		delete m_ShadowTexture;
		m_ShadowTexture = 0;
	}

	if (m_DeferredBuffers)
	{
		m_DeferredBuffers->Shutdown();
		delete m_DeferredBuffers;
		m_DeferredBuffers = 0;
	}

	if (m_FullScreenWindow)
	{
		m_FullScreenWindow->Shutdown();
		delete m_FullScreenWindow;
		m_FullScreenWindow = 0;
	}

	if (m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}

	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = NULL;
	}
}


D3DClass* Graphics::GetD3DClass()
{
	return m_Direct3D;
}

void Graphics::RenderTessellation(RenderType render)
{
	switch (render)
	{
		case RenderType::SHADOW:
		{

		}
			break;
		case RenderType::DEFERRED :
		{
			XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
			XMFLOAT3 cameraPosition, modelPosition;

			//m_Camera->Render();

			m_Direct3D->GetWorldMatrix(worldMatrix);
			m_Camera[0]->GetViewMatrix(viewMatrix);
			m_Direct3D->GetProjectionMatrix(projectionMatrix);

			cameraPosition = m_Camera[m_CameraNumber]->GetPosition();

			for (auto modelT : m_modelTessellations)
			{

				//Model Load
				modelT->Render(m_Direct3D->GetDeviceContext());

				//draw to deferred buffer
				if (!m_ShaderManager->RenderTessellationShader(m_Direct3D->GetDeviceContext(), modelT->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
					g_data->tessellationAmount))
				{
					return;
				}
			}
		}
			break;
		case RenderType::FORWARD:
		{

		}
			break;
		default: break;
		
	}


}

void Graphics::RenderBillboard(RenderType render)
{
	switch (render)
	{
	case RenderType::SHADOW:
	{

	}
	break;
	case RenderType::DEFERRED:
	{
		XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
		XMFLOAT3 cameraPosition, modelPosition;


		m_Direct3D->GetWorldMatrix(worldMatrix);
		m_Camera[m_CameraNumber]->GetViewMatrix(viewMatrix);
		m_Direct3D->GetProjectionMatrix(projectionMatrix);

		cameraPosition = m_Camera[m_CameraNumber]->GetPosition();

		for (auto modelB : m_modelBillboards)
		{
			modelPosition = modelB->m_transform->getPosition();
			double angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (180.0 / XM_PI);

			float rotation = (float)angle * 0.0174532925f;

			worldMatrix = XMMatrixRotationY(rotation);

			// translation
			translateMatrix = XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);
			worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

			//Model Load
			modelB->Render(m_Direct3D->GetDeviceContext());

			//draw to deferred buffer
			m_ShaderManager->RenderDeferredShader(m_Direct3D->GetDeviceContext(), modelB->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				modelB->GetTexture(DEFAULT));
		}
	}
	break;
	case RenderType::FORWARD:
	{

	}
	break;
	default: break;

	}
	
}

void Graphics::RenderModels(RenderType render)
{
	switch (render)
	{
	case RenderType::SHADOW:
	{
		XMMATRIX worldMatrix, lightViewMatrix, lightOrthoMatrix, viewMatrix, projectionMatrix;
		XMMATRIX multiply;

		m_Direct3D->GetWorldMatrix(worldMatrix);
		//m_Camera[m_CameraNumber]->GetViewMatrix(viewMatrix);
		//m_Direct3D->GetProjectionMatrix(projectionMatrix);

		m_dirLights[m_shadowNum]->GenerateViewMatrix();
		m_dirLights[m_shadowNum]->GetViewMatrix(lightViewMatrix);
		m_dirLights[m_shadowNum]->GetOrthoMatrix(lightOrthoMatrix);

		for (auto& model : m_models)
		{
			m_Direct3D->GetWorldMatrix(worldMatrix);
			XMFLOAT3 trans;
			trans = model->m_transform->getScale();
			multiply = XMMatrixScaling(trans.x, trans.y, trans.z);
			worldMatrix = XMMatrixMultiply(worldMatrix, multiply);

			trans = model->m_transform->getRotation();

			multiply = XMMatrixRotationRollPitchYaw(XMConvertToRadians(trans.x), XMConvertToRadians(trans.y), XMConvertToRadians(trans.z));
			worldMatrix = XMMatrixMultiply(worldMatrix, multiply);

			trans = model->m_transform->getPosition();
			multiply = XMMatrixTranslation(trans.x, trans.y, trans.z);
			worldMatrix = XMMatrixMultiply(worldMatrix, multiply);

			//Model Load
			model->Render(m_Direct3D->GetDeviceContext());
			//draw to deferred buffer
			if (!m_ShaderManager->RenderDepthShader(m_Direct3D->GetDeviceContext(), model->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix))
			{
				return;
			}
		}
	}
	break;
	case RenderType::DEFERRED:
	{
		XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
		XMMATRIX multiply;

		m_Direct3D->GetWorldMatrix(worldMatrix);
		m_Camera[m_CameraNumber]->GetViewMatrix(viewMatrix);
		m_Direct3D->GetProjectionMatrix(projectionMatrix);

		for (auto& model : m_models)
		{
			m_Direct3D->GetWorldMatrix(worldMatrix);
			XMFLOAT3 trans;
			trans = model->m_transform->getScale();
			multiply = XMMatrixScaling(trans.x, trans.y, trans.z);
			worldMatrix = XMMatrixMultiply(worldMatrix, multiply);

			trans = model->m_transform->getRotation();

			multiply = XMMatrixRotationRollPitchYaw(XMConvertToRadians(trans.x), XMConvertToRadians(trans.y), XMConvertToRadians(trans.z));
			worldMatrix = XMMatrixMultiply(worldMatrix, multiply);

			trans = model->m_transform->getPosition();
			multiply = XMMatrixTranslation(trans.x, trans.y, trans.z);
			worldMatrix = XMMatrixMultiply(worldMatrix, multiply);

			//Model Load
			model->Render(m_Direct3D->GetDeviceContext());
			//draw to deferred buffer
			if (!m_ShaderManager->RenderDeferredShader(m_Direct3D->GetDeviceContext(), model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, model->GetTexture(DEFAULT)))
			{
				return;
			}
		}
	}
	break;
	case RenderType::FORWARD:
	{

	}
	break;
	default: break;

	}
}

void Graphics::RenderModelInstances(RenderType render)
{
	switch (render)
	{
	case RenderType::SHADOW:
	{
		XMMATRIX worldMatrix, lightViewMatrix, lightOrthoMatrix, viewMatrix, projectionMatrix;
		XMMATRIX multiply;

		m_Direct3D->GetWorldMatrix(worldMatrix);
		//m_Camera[m_CameraNumber]->GetViewMatrix(viewMatrix);
		//m_Direct3D->GetProjectionMatrix(projectionMatrix);

		m_dirLights[m_shadowNum]->GenerateViewMatrix();
		m_dirLights[m_shadowNum]->GetViewMatrix(lightViewMatrix);
		m_dirLights[m_shadowNum]->GetOrthoMatrix(lightOrthoMatrix);

		for (auto& modelinst : m_modelInstances)
		{
			m_Direct3D->GetWorldMatrix(worldMatrix);
			XMFLOAT3 trans;
			trans = modelinst->m_transform->getScale();
			multiply = XMMatrixScaling(trans.x, trans.y, trans.z);
			worldMatrix = XMMatrixMultiply(worldMatrix, multiply);

			trans = modelinst->m_transform->getRotation();

			multiply = XMMatrixRotationRollPitchYaw(XMConvertToRadians(trans.x), XMConvertToRadians(trans.y), XMConvertToRadians(trans.z));
			worldMatrix = XMMatrixMultiply(worldMatrix, multiply);

			trans = modelinst->m_transform->getPosition();
			multiply = XMMatrixTranslation(trans.x, trans.y, trans.z);
			worldMatrix = XMMatrixMultiply(worldMatrix, multiply);

			//Model Load
			modelinst->Render(m_Direct3D->GetDeviceContext());
			//draw to deferred buffer
			if (!m_ShaderManager->RenderDepthInstanceShader(m_Direct3D->GetDeviceContext(), modelinst->GetIndexCount(), modelinst->GetInstanceCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix))
			{
				return;
			}
		}
	}
	break;
	case RenderType::DEFERRED:
	{
		XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
		XMMATRIX multiply;

		m_Direct3D->GetWorldMatrix(worldMatrix);
		m_Camera[m_CameraNumber]->GetViewMatrix(viewMatrix);
		m_Direct3D->GetProjectionMatrix(projectionMatrix);

		for (auto& modelinst : m_modelInstances)
		{
			m_Direct3D->GetWorldMatrix(worldMatrix);
			XMFLOAT3 trans;
			trans = modelinst->m_transform->getScale();
			multiply = XMMatrixScaling(trans.x, trans.y, trans.z);
			worldMatrix = XMMatrixMultiply(worldMatrix, multiply);

			trans = modelinst->m_transform->getRotation();

			multiply = XMMatrixRotationRollPitchYaw(XMConvertToRadians(trans.x), XMConvertToRadians(trans.y), XMConvertToRadians(trans.z));
			worldMatrix = XMMatrixMultiply(worldMatrix, multiply);

			trans = modelinst->m_transform->getPosition();
			multiply = XMMatrixTranslation(trans.x, trans.y, trans.z);
			worldMatrix = XMMatrixMultiply(worldMatrix, multiply);

			//Model Load
			modelinst->Render(m_Direct3D->GetDeviceContext());
			//draw to deferred buffer
			if (!m_ShaderManager->RenderTextureInstanceShader(m_Direct3D->GetDeviceContext(), modelinst->GetIndexCount(), modelinst->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, modelinst->GetTexture(DEFAULT)))
			{
				return;
			}
		}
	}
	break;
	case RenderType::FORWARD:
	{

	}
	break;
	default: break;

	}
}

void Graphics::RenderDeferredToTexture()
{
	// Set render buffer to render target 
	m_DeferredBuffers->SetRenderTargets(m_Direct3D->GetDeviceContext());

	// Clear render buffer
	m_DeferredBuffers->ClearRenderTargets(m_Direct3D->GetDeviceContext(), 0, 0, 0, 1.0f);

	//Render 
	RenderTessellation(RenderType::DEFERRED);

	RenderBillboard(RenderType::DEFERRED);

	RenderModelInstances(RenderType::DEFERRED);
	
	RenderModels(RenderType::DEFERRED);


	// Reset render target back
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset viewport back
	m_Direct3D->ResetViewport();
	
}

void Graphics::RenderDeferred()
{
	XMMATRIX worldMatrix, baseViewMatrix, viewMatrix, orthoMatrix, projectionMatrix;


	//m_Direct3D->BeginScene(g_data->clear_color[0], g_data->clear_color[1], g_data->clear_color[2], 1.0f);
	
	// Get Matrices
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera[m_CameraNumber]->GetBaseViewMatrix(baseViewMatrix);
	m_Camera[m_CameraNumber]->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	//m_Camera->Render();
	// Turn off Zbuffer to begin 2D rendering
	m_Direct3D->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	m_ShaderManager->RenderDeferredMultiLightShader(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_DeferredBuffers->GetShaderResourceView(0), m_DeferredBuffers->GetShaderResourceView(1), m_DeferredBuffers->GetShaderResourceView(2), m_DeferredBuffers->GetShaderResourceView(3), m_dirLights, m_pointLights, m_spotLights, m_ShadowTexture->GetShaderResourceView());

	// Turn Zbuffer back
	m_Direct3D->TurnZBufferOn();

	
}

void Graphics::RenderResources()
{
	XMMATRIX worldMatrix, baseViewMatrix, viewMatrix, orthoMatrix, projectionMatrix;

	// Get Matrices
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera[m_CameraNumber]->GetBaseViewMatrix(baseViewMatrix);
	m_Camera[m_CameraNumber]->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Turn off Zbuffer to begin 2D rendering
	m_Direct3D->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	switch(g_data->resourceN)
	{
	case 1:
		//Resource 0 : POSITION
		if (!m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
			m_DeferredBuffers->GetShaderResourceView(0))) return;
		break;
	case 2:
		//Resource 1 : DIFFUSE
		if (!m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
			m_DeferredBuffers->GetShaderResourceView(1))) return;
		break;
	case 3:
		//Resource 2 : NORMAL
		if (!m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
			m_DeferredBuffers->GetShaderResourceView(2))) return;
		break;
	case 4:
		//Resource 2 : NORMAL
		if (!m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
			m_DeferredBuffers->GetShaderResourceView(3))) return;
		break;
	default :
		//Resource DEFAULT : POSITION
		if (!m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
			m_DeferredBuffers->GetShaderResourceView(0))) return;
	}


	// Turn Zbuffer back
	m_Direct3D->TurnZBufferOn();

}

void Graphics::RenderShadows()
{
	XMMATRIX worldMatrix, baseViewMatrix, viewMatrix, orthoMatrix, projectionMatrix;

	// Get Matrices
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera[m_CameraNumber]->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Turn off Zbuffer to begin 2D rendering
	m_Direct3D->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());
	int picknum = min(g_data->resourceShadow, MAX_DIRLIGHTS) - 1;

	if (!m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_ShadowTexture->GetShaderResourceView(picknum))) return;

	// Turn Zbuffer back
	m_Direct3D->TurnZBufferOn();
}

//TODO : Forward Render
void Graphics::RenderNormal()
{
	RenderTessellation(RenderType::FORWARD);

	RenderBillboard(RenderType::FORWARD);

	RenderModelInstances(RenderType::FORWARD);

	RenderModels(RenderType::FORWARD);
}


// Shadow depth
void Graphics::MakeShadow()
{
	m_shadowNum = 0;

	for (int i = 0; i < m_dirLights.size(); i++)
	{
		m_ShadowTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), i);

		m_ShadowTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f, i);

		RenderModels(RenderType::SHADOW);

		RenderModelInstances(RenderType::SHADOW);


		m_Direct3D->SetBackBufferRenderTarget();
		m_Direct3D->ResetViewport();
		m_shadowNum++;
	}
}



//Initialize IMGUI
void Graphics::ImguiInitialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplWin32_Init(mAPPLICATION->GetHWND());
	ImGui_ImplDX11_Init(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
	ImGui::StyleColorsDark();
}

// Load data from  g_data to imgui
void Graphics::ImguiLoadInfo()
{
	int i = 0;
	g_data->CameraNumber = m_CameraNumber;

	for (auto model : m_models)
	{
		g_data->cubePosition[i][0] = model->m_transform->getPosition().x;
		g_data->cubePosition[i][1] = model->m_transform->getPosition().y;
		g_data->cubePosition[i][2] = model->m_transform->getPosition().z;
		i++;
	}
	i = 0;
	for (auto model : m_models)
	{
		g_data->cubeRotation[i][0] = model->m_transform->getRotation().x;
		g_data->cubeRotation[i][1] = model->m_transform->getRotation().y;
		g_data->cubeRotation[i][2] = model->m_transform->getRotation().z;
		i++;
	}

	i = 0;
	for (auto model : m_modelInstances)
	{
		g_data->cubePosition2[i][0] = model->m_transform->getPosition().x;
		g_data->cubePosition2[i][1] = model->m_transform->getPosition().y;
		g_data->cubePosition2[i][2] = model->m_transform->getPosition().z;
		i++;
	}
	i = 0;
	for (auto model : m_modelBillboards)
	{
		g_data->billboardPosition[i][0] = model->m_transform->getPosition().x;
		g_data->billboardPosition[i][1] = model->m_transform->getPosition().y;
		g_data->billboardPosition[i][2] = model->m_transform->getPosition().z;
		i++;
	}

	i = 0;
	for (auto DirL : m_dirLights)
	{
		g_data->DirLightDirection[i][0] = DirL->m_direction.x;
		g_data->DirLightDirection[i][1] = DirL->m_direction.y;
		g_data->DirLightDirection[i][2] = DirL->m_direction.z;
		i++;
	}

	i = 0;
	for (auto PointL : m_pointLights)
	{
		g_data->PointLightPosition[i][0] = PointL->m_position.x;
		g_data->PointLightPosition[i][1] = PointL->m_position.y;
		g_data->PointLightPosition[i][2] = PointL->m_position.z;
		i++;
	}

	i = 0;
	for (auto spotL : m_spotLights)
	{
		g_data->SpotLightPosition[i][0] = spotL->m_position.x;
		g_data->SpotLightPosition[i][1] = spotL->m_position.y;
		g_data->SpotLightPosition[i][2] = spotL->m_position.z;
		i++;
	}

	i = 0;
	for (auto spotL : m_spotLights)
	{
		g_data->SpotLightDirection[i][0] = spotL->m_direction.x;
		g_data->SpotLightDirection[i][1] = spotL->m_direction.y;
		g_data->SpotLightDirection[i][2] = spotL->m_direction.z;
		i++;
	}
}


//IMGUI Update
void Graphics::ImguiUpdate(float dt)
{
	ImVec4 clearVec4 = ImVec4(g_data->clear_color[0], g_data->clear_color[1], g_data->clear_color[2], 1.0f);
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	std::string name;
	int i = 0;

	ImguiLoadInfo();

	ImGui::Begin("Hello, world!");
	ImGui::Text("Hello Whatsseob.");               // Test text
	ImGui::Checkbox("Deferred", &g_data->isDeferred);      // bool box. Decide deferred or forward
	ImGui::SliderInt("RenderList", &g_data->renderlist, 0, 3);      // pick int. Decide render type
	m_renderlist = (RenderList)g_data->renderlist;
	ImGui::SliderInt("resourceNumber", &g_data->resourceN, 1, 5);      // pick int. Decide resourece number
	ImGui::SliderInt("resourceShadowNumber", &g_data->resourceShadow, 1, MAX_DIRLIGHTS);      // pick int. Decide resourece number
	ImGui::SliderInt("Camera Number", &g_data->CameraNumber, 1, m_Camera.size());      // pick int. Pick Camera number. 
	m_CameraNumber = g_data->CameraNumber;
	ImGui::SliderFloat("Tessellation Amount", &g_data->tessellationAmount, 1.0f, 10.0f);            // float slider. set tessellation amount
	ImGui::ColorEdit3("clear color", (float*)&clearVec4); // clear color
	g_data->clear_color[0] = clearVec4.x;
	g_data->clear_color[1] = clearVec4.y;
	g_data->clear_color[2] = clearVec4.z;

	// Model position
	for (auto model : m_models)
	{
		name = model->GetOwner()->GetName() + " m  position";
		char* cname = const_cast<char*>(name.c_str());
		ImGui::SliderFloat3(cname, g_data->cubePosition[i], -5.0f, 5.0f);
		model->m_transform->setPosition(g_data->cubePosition[i][0], g_data->cubePosition[i][1], g_data->cubePosition[i][2]);
		i++;
	}
	i = 0;
	// Model rotation
	for (auto model : m_models)
	{
		name = model->GetOwner()->GetName() + " m  rotation";
		char* cname = const_cast<char*>(name.c_str());
		ImGui::SliderFloat3(cname, g_data->cubeRotation[i], -180.0f, 180.0f);
		model->m_transform->setRotation(g_data->cubeRotation[i][0], g_data->cubeRotation[i][1], g_data->cubeRotation[i][2]);
		i++;
	}
	i = 0;
	for (auto model : m_modelInstances)
	{
		name = model->GetOwner()->GetName() + " Inst m  position";
		char* cname = const_cast<char*>(name.c_str());
		ImGui::SliderFloat3(cname, g_data->cubePosition2[i], -5.0f, 5.0f);
		model->m_transform->setPosition(g_data->cubePosition2[i][0], g_data->cubePosition2[i][1], g_data->cubePosition2[i][2]);
		i++;
	}
	i = 0;
	for (auto model : m_modelBillboards)
	{
		name = model->GetOwner()->GetName() + " Bill m  position";
		char* cname = const_cast<char*>(name.c_str());
		ImGui::SliderFloat3(cname, g_data->billboardPosition[i], -5.0f, 5.0f);
		model->m_transform->setPosition(g_data->billboardPosition[i][0], g_data->billboardPosition[i][1], g_data->billboardPosition[i][2]);
		i++;
	}

	i = 0;
	for (auto DirL : m_dirLights)
	{
		name = "Dir Light Dir " + to_string(i);
		char* cname = const_cast<char*>(name.c_str());
		ImGui::SliderFloat3(cname, g_data->DirLightDirection[i], -5.0f, 5.0f);
		DirL->m_direction = XMFLOAT3(g_data->DirLightDirection[i][0], g_data->DirLightDirection[i][1], g_data->DirLightDirection[i][2]);
		i++;
	}

	i = 0;
	for (auto PointL : m_pointLights)
	{
		name = "Point Light Pos " + to_string(i);
		char* cname = const_cast<char*>(name.c_str());
		ImGui::SliderFloat3(cname, g_data->PointLightPosition[i], -5.0f, 5.0f);
		PointL->m_position = XMFLOAT3(g_data->PointLightPosition[i][0], g_data->PointLightPosition[i][1], g_data->PointLightPosition[i][2]);
		i++;
	}
	i = 0;
	for (auto spotL : m_spotLights)
	{
		name = "Spot Light Pos " + to_string(i);
		char* cname = const_cast<char*>(name.c_str());
		ImGui::SliderFloat3(cname, g_data->SpotLightPosition[i], -5.0f, 5.0f);
		spotL->m_position = XMFLOAT3(g_data->SpotLightPosition[i][0], g_data->SpotLightPosition[i][1], g_data->SpotLightPosition[i][2]);
		i++;
	}
	i = 0;
	for (auto spotL : m_spotLights)
	{
		name = "Spot Light Dir " + to_string(i);
		char* cname = const_cast<char*>(name.c_str());
		ImGui::SliderFloat3(cname, g_data->SpotLightDirection[i], -5.0f, 5.0f);
		spotL->m_direction = XMFLOAT3(g_data->SpotLightDirection[i][0], g_data->SpotLightDirection[i][1], g_data->SpotLightDirection[i][2]);
		i++;
	}

	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		g_data->counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", g_data->counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("dt :  %.4f ", dt);

	ImGui::End();

	// Show another simple window.
	if (g_data->show_another_window)
	{
		ImGui::Begin("Another Window", &g_data->show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");

		ImGui::Text("Mouse Mover : %.1f , %.1f ", mINPUT->mouseMover[0], mINPUT->mouseMover[1]);

		if (ImGui::Button("Close Me"))
			g_data->show_another_window = false;
		ImGui::End();
	}

}

void Graphics::ImguiShutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Graphics::InitializeLightInfo()
{
	m_ShaderManager->InitializeLightBuffer(m_dirLights, m_pointLights, m_spotLights);
}
