#pragma once
#include "Component/Component.h"
#include "Systems/Graphics/Graphics.h"

class Graphics;

class DirLight : public Component
{
public:
	DirLight();
	~DirLight();

	void Initialize();

	//Direction Light Component
	XMFLOAT3 m_position;
	XMFLOAT3 m_direction;
	XMFLOAT4 m_ambient;
	XMFLOAT4 m_diffuse;


	//For Shadow/Depth
	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float, float);

	void GetViewMatrix(XMMATRIX&);
	void GetProjectionMatrix(XMMATRIX&);

	void GenerateOrthoMatrix(float, float, float);
	void GenerateOrthoMatrix(float, float, float, float);
	void GetOrthoMatrix(XMMATRIX&);

	void SetShadow(bool);

	XMMATRIX GetVPMatrix(bool isProjection);

private:

	//For Shadow/Depth
	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_orthoMatrix;

	XMFLOAT3 m_lookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);
	bool isMakeShadow = false;
};