#pragma once
#include "Component/Component.h"
#include "Systems/Graphics/Graphics.h"

class Graphics;

class PointLight : public Component
{
public:
	PointLight();
	~PointLight();

	void Initialize();

	//Point Light Component
	XMFLOAT3 m_position;
	XMFLOAT4 m_diffuse;
	XMFLOAT4 m_ambient;
	float m_constant;
	float m_linear;
	float m_quadratic;

	//For Shadow/Depth
	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float, float);

	void GetViewMatrix(XMMATRIX&);
	void GetProjectionMatrix(XMMATRIX&);

	void GenerateOrthoMatrix(float, float, float);
	void GenerateOrthoMatrix(float, float, float, float);
	void GetOrthoMatrix(XMMATRIX&);

private:

	//For Shadow/Depth
	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_orthoMatrix;

	bool isMakeShadow = false;

};