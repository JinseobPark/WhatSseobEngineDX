#include "stdafx.h"
#include "PointLight.h"
#include "Component/ComponentHeaders.h"
#include "Object/ObjectClass.h"

PointLight::PointLight() : Component(CT_POINT_LIGHT)
{
	//Initialize component
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_ambient = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
	m_constant = 1.0f;
	m_linear = 0.09f;
	m_quadratic = 0.032f;

	//Add Point light vector
	mGRAPHICS->m_pointLights.push_back(this);
}

PointLight::~PointLight()
{
	//delete Point light vector
	mGRAPHICS->m_pointLights.erase(std::find(mGRAPHICS->m_pointLights.begin(), mGRAPHICS->m_pointLights.end(), this));
}

void PointLight::Initialize()
{

}