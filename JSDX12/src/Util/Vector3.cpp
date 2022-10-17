#include "stdafx.h"
#include "vector3.h"
#include <cmath>

Vector3::Vector3() :x(0.0f), y(0.0f), z(0.0f)
{

}
Vector3::Vector3(float xV, float yV, float zV) : x(xV), y(yV), z(zV)
{

}

Vector3::~Vector3()
{
}

void Vector3::Set(float xV, float yV, float zV)
{
	x = xV;
	y = yV;
	z = zV;
}
void Vector3::Set(const Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

void Vector3::SetZero()
{
	x = 0.f;
	y = 0.f;
	z = 0.f;
}

bool Vector3::IsZero() const
{
	return (x == 0.f) && (y == 0.f) && (z == 0.f);
}
bool Vector3::IsOne() const
{
	return (x == 1.f) && (y == 1.f) && (z == 1.f);
}

float Vector3::DotProduct(const Vector3& v) const
{
	return x * v.x + y * v.y + z * v.z;
}
Vector3 Vector3::CrossProduct(const Vector3& v) const
{
	Vector3 result;
	result.x = y * v.z - z * v.y;
	result.y = z * v.x - x * v.z;
	result.z = x * v.y - y * v.x;


	return result;
}


float Vector3::LengthSq()
{
	return x * x + y * y + z * z;
}
float Vector3::Length()
{
	return sqrt(LengthSq());
}

float Vector3::DistanceSq(const Vector3& v) const
{
	return (*this - v).LengthSq();
}
float Vector3::Distance(const Vector3& v) const
{
	return (*this - v).Length();
}

Vector3& Vector3::Normalize()
{
	float n = Length();
	if (n == 1.f || n == 0.f)
		return *this;
	n = 1.f / n;

	x *= n;
	y *= n;
	z *= n;

	return *this;
}



/************************* operator ***********************/
Vector3 Vector3::operator+(const Vector3& v) const
{
	return Vector3(x + v.x, y + v.y, z + v.z);
}
Vector3 Vector3::operator-(const Vector3& v) const
{
	return Vector3(x - v.x, y - v.y, z - v.z);
}
Vector3& Vector3::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}
Vector3& Vector3::operator-=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}


Vector3 Vector3::operator*(float s) const
{
	return Vector3(x * s, y * s, z * s);
}
Vector3& Vector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;

	return *this;
}

Vector3 Vector3::operator/(float s) const
{
	s = 1.f / s;
	return Vector3(x * s, y * s, z * s);
}
Vector3& Vector3::operator/=(float s)
{
	s = 1.f / s;
	x *= s;
	y *= s;
	z *= s;

	return *this;
}

Vector3 Vector3::operator-()const
{
	return Vector3(-x, -y, -z);
}

bool Vector3::operator==(const Vector3& v) const
{
	return (x == v.x) && (y == v.y) && (z == v.z);
}
bool Vector3::operator!=(const Vector3& v) const
{
	return (x != v.x) || (y != v.y) || (z != v.z);
}

