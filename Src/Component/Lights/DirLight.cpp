#include "stdafx.h"
#include "DirLight.h"
#include "Component/ComponentHeaders.h"
#include "Object/ObjectClass.h"

DirLight::DirLight() : Component(CT_DIR_LIGHT)
{
	//Initialize component
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_ambient = XMFLOAT4(0.03f, 0.03f, 0.03f, 1.0f);

	//Add direction light vector
	mGRAPHICS->m_dirLights.push_back(this);
}

DirLight::~DirLight()
{
	//delete direction light vector
	mGRAPHICS->m_dirLights.erase(std::find(mGRAPHICS->m_dirLights.begin(), mGRAPHICS->m_dirLights.end(), this));
}

void DirLight::Initialize()
{

}


void DirLight::GenerateViewMatrix()
{
	// Set Up vector
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	XMVECTOR upVector = XMLoadFloat3(&up);
	XMVECTOR positionVector = XMLoadFloat3(&m_position);
	XMVECTOR lookAtVector = XMLoadFloat3(&m_direction);

	//Make view matrix
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}


void DirLight::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	// set aspect and FOV
	float fieldOfView = (float)XM_PI / 2.0f;
	float screenAspect = 1.0f;

	// Make projection matrix by light
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
}


void DirLight::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}


void DirLight::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void DirLight::GenerateOrthoMatrix(float width, float depthPlane, float nearPlane)
{
	// Make orthomatrix by light
	m_orthoMatrix = XMMatrixOrthographicLH(width, width, nearPlane, depthPlane);
}

void DirLight::GenerateOrthoMatrix(float width, float height, float screenDepth, float screenNear)
{
	// Make projection matrix by light
	m_orthoMatrix = XMMatrixOrthographicLH(width, height, screenNear, screenDepth);
}

void DirLight::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}