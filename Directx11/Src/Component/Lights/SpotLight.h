#pragma once
#include "Component/Component.h"
#include "Systems/Graphics/Graphics.h"

class Graphics;

class SpotLight : public Component
{
public:
	SpotLight();
	~SpotLight();

	void Initialize();


	XMFLOAT3 m_position;
	XMFLOAT3 m_direction;
	XMFLOAT4 m_diffuse;
	XMFLOAT4 m_ambient;
	float m_cutOff;
	float m_outerCutOff;
	float m_constant;
	float m_linear;
	float m_quadratic;


	//depth
	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float, float);

	void GetViewMatrix(XMMATRIX&);
	void GetProjectionMatrix(XMMATRIX&);

	void GenerateOrthoMatrix(float, float, float);
	void GenerateOrthoMatrix(float, float, float, float);
	void GetOrthoMatrix(XMMATRIX&);

private:

	//depth
	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_orthoMatrix;
};
