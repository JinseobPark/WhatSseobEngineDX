#pragma once
class CameraClass : public AlignedAllocationPolicy<16>
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetPosition(XMFLOAT3 p);
	void SetRotation(float, float, float);
	void SetRotation(XMFLOAT3 p);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);

	void RenderReflection(float);
	XMMATRIX GetReflectionViewMatrix();

	void RenderBaseViewMatrix();
	void GetBaseViewMatrix(XMMATRIX&);

private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_reflectionViewMatrix;
	XMMATRIX m_baseViewMatrix;
};