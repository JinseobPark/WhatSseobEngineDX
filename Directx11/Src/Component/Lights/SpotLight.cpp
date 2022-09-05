#include "stdafx.h"
#include "SpotLight.h"
#include "Component/ComponentHeaders.h"
#include "Object/ObjectClass.h"

SpotLight::SpotLight() : Component(CT_SPOT_LIGHT)
{
	//Initialize component
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_cutOff = cos(XMConvertToRadians(12.5f));
	m_outerCutOff = cos(XMConvertToRadians(15.0f));
	m_constant = 1.0f;
	m_linear = 0.09f;
	m_quadratic = 0.032f;

	//Add Spot light vector
	mGRAPHICS->m_spotLights.push_back(this);
}

SpotLight::~SpotLight()
{
	//delete Spot light vector
	mGRAPHICS->m_spotLights.erase(std::find(mGRAPHICS->m_spotLights.begin(), mGRAPHICS->m_spotLights.end(), this));
}

void SpotLight::Initialize()
{

}
