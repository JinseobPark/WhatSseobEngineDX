#include "stdafx.h"
#include "PositionClass.h"


PositionClass::PositionClass()
{
}


PositionClass::PositionClass(const PositionClass& other)
{
}


PositionClass::~PositionClass()
{
}


void PositionClass::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}


void PositionClass::SetRotation(float x, float y, float z)
{
	m_rotation = XMFLOAT3(x, y, z);
}


void PositionClass::GetPosition(XMFLOAT3& p)
{
	p = m_position;
}


void PositionClass::GetRotation(XMFLOAT3& p)
{
	p = m_rotation;
}


void PositionClass::SetFrameTime(float time)
{
	m_frameTime = time;
}


void PositionClass::MoveLeft(bool keydown)
{
	if (keydown)
		m_position.x -= 0.1;
}


void PositionClass::MoveRight(bool keydown)
{
	// 키를 누르면 카메라가 오른쪽으로 회전하는 속도가 증가합니다. 회전 속도를 늦추지 않으면.
	if (keydown)
	m_position.x += 0.1;
}

void PositionClass::MoveUp(bool keydown)
{
	if (keydown)
	m_position.y += 0.1;
}

void PositionClass::MoveDown(bool keydown)
{
	if (keydown)
	m_position.y -= 0.1;
}

void PositionClass::MoveForward(bool keydown)
{
	if (keydown)
	m_position.z += 0.1;
}

void PositionClass::MoveBack(bool keydown)
{
	if (keydown)
	m_position.z -= 0.1;
}
