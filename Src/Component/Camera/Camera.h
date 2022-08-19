#pragma once
#include "Systems/Graphics/Graphics.h"
#include "Component/Component.h"
#include "Component/ComponentHeaders.h"
class Camera : public Component
{
public:
	Camera();
	~Camera();

	void Initialize() override;

	//Get Set
	void SetPosition(float, float, float);
	void SetPosition(XMFLOAT3 p);
	void SetRotation(float, float, float);
	void SetRotation(XMFLOAT3 p);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);

	//Reflection
	void RenderReflection(float);
	XMMATRIX GetReflectionViewMatrix();

	//Base View Matrix
	void RenderBaseViewMatrix();
	void GetBaseViewMatrix(XMMATRIX&);

	//Camera Move
	void Rotation(LONG, LONG);

private:

	friend class Graphics;
	Transform* m_transform = nullptr;

	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;

	XMMATRIX m_reflectionViewMatrix;
	XMMATRIX m_baseViewMatrix;

};