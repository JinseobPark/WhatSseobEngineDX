#include "stdafx.h"
#include "camera.h"
#include "Object/ObjectClass.h"



Camera::Camera() : Component(CT_CAMERA)
{
	//add camera to  graphics camera vector
	mGRAPHICS->m_Camera.push_back(this);
}


Camera::~Camera()
{
	//delete of camera vector
	mGRAPHICS->m_Camera.erase(std::find(mGRAPHICS->m_Camera.begin(), mGRAPHICS->m_Camera.end(), this));
}


void Camera::Initialize()
{
	m_transform = (Transform*)GetOwner()->GetComponent(CT_TRANSFORM);
	SetPosition(0.0f, 2.0f, -10.0f);
	RenderBaseViewMatrix();
}

void Camera::SetPosition(float x, float y, float z)
{
	m_transform->position.x = x;
	m_transform->position.y = y;
	m_transform->position.z = z;
}


void Camera::SetRotation(float x, float y, float z)
{
	m_transform->rotation.x = x;
	m_transform->rotation.y = y;
	m_transform->rotation.z = z;
}

void Camera::SetPosition(XMFLOAT3 p)
{
	m_transform->position = p;
}


void Camera::SetRotation(XMFLOAT3 p)
{
	m_transform->rotation = p;
}

XMFLOAT3 Camera::GetPosition()
{
	return m_transform->position;
}


XMFLOAT3 Camera::GetRotation()
{
	return m_transform->rotation;
}

//Render Camera with camera transform
void Camera::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// set up vector 
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	upVector = XMLoadFloat3(&up);

	// set position
	position = m_transform->position;
	positionVector = XMLoadFloat3(&position);

	// set look vector
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookAtVector = XMLoadFloat3(&lookAt);

	// Calculate the rotation in radians.
	pitch = m_transform->rotation.x * 0.0174532925f;
	yaw = m_transform->rotation.y * 0.0174532925f;
	roll = m_transform->rotation.z * 0.0174532925f;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// set View matrix
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}


void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

//For Reflection camera by y-axis
void Camera::RenderReflection(float height)
{
	XMFLOAT3 up, position, lookAt;

	// set up vector 
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	XMVECTOR upVector = XMLoadFloat3(&up);

	// set position
	position.x = m_transform->position.x;
	position.y = -m_transform->position.y + (height * 2.0f);
	position.z = m_transform->position.z;

	XMVECTOR positionVector = XMLoadFloat3(&position);

	// Calculate the rotation in radians.
	float radians = m_transform->rotation.y * 0.0174532925f;

	// set look vector
	lookAt.x = sinf(radians) + m_transform->position.x;
	lookAt.y = position.y;
	lookAt.z = cosf(radians) + m_transform->position.z;

	XMVECTOR lookAtVector = XMLoadFloat3(&lookAt);

	// set View matrix
	m_reflectionViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}


XMMATRIX Camera::GetReflectionViewMatrix()
{
	return m_reflectionViewMatrix;
}


void Camera::RenderBaseViewMatrix()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;

	// Set up vector
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVector = XMLoadFloat3(&up);

	// Set position
	position = m_transform->position;

	positionVector = XMLoadFloat3(&position);

	// Calculate the rotation in radians.
	float radians = m_transform->rotation.y * 0.0174532925f;
	
	// set look vector
	lookAt.x = sinf(radians) + m_transform->position.x;
	lookAt.y = m_transform->position.y;
	lookAt.z = cosf(radians) + m_transform->position.z;

	lookAtVector = XMLoadFloat3(&lookAt);

	// set View matrix
	m_baseViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}


void Camera::GetBaseViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_baseViewMatrix;
}

//Camera Rotation by mouse
void Camera::Rotation(LONG _x, LONG _y)
{
	m_transform->rotation.x += (float)_x * 0.3f;
	m_transform->rotation.y += (float)_y * 0.3f;
	Render();
}
