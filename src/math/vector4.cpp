#include "../../src/pch.h"
#include "vector4.h"

#include "math.h"

vector4::vector4()
{
	vector4::vector4(0.0f, 0.0f, 0.0f, 0.0f);
}

vector4::vector4(float scalar)
{
	vector4::vector4(scalar, scalar, scalar, scalar);
}

vector4::vector4(float x, float y, float z, float w)
{
	m_value = math::simd_create(x, y, z, w);
}

void vector4::add(vector4& a)
{
	m_value = math::simd_add(m_value, a.m_value);
}

void vector4::sub(vector4& a)
{
	m_value = math::simd_sub(m_value, a.m_value);
}

void vector4::div(vector4& a)
{
	m_value = math::simd_div(m_value, a.m_value);
}

void vector4::mul(vector4& a)
{
	m_value = math::simd_mul(m_value, a.m_value);
}

float vector4::x()
{
	return m_value.m128_f32[0];
}

float vector4::y()
{
	return m_value.m128_f32[1];
}

float vector4::z()
{
	return m_value.m128_f32[2];
}

float vector4::w()
{
	return m_value.m128_f32[3];
}

vector4& vector4::operator+=(const vector4& rhs)
{
	m_value = math::simd_add(m_value, rhs.m_value);
	return *this;
}

vector4& vector4::operator-=(const vector4& rhs)
{
	m_value = math::simd_sub(m_value, rhs.m_value);
	return *this;
}

vector4& vector4::operator/=(const vector4& rhs)
{
	m_value = math::simd_div(m_value, rhs.m_value);
	return *this;
}

vector4& vector4::operator*=(const vector4& rhs)
{
	m_value = math::simd_mul(m_value, rhs.m_value);
	return *this;
}

vector4 operator+(vector4 lhs, const vector4& rhs)
{
	lhs += rhs;
	return lhs;
}

vector4 operator-(vector4 lhs, const vector4& rhs)
{
	lhs -= rhs;
	return lhs;
}

vector4 operator/(vector4 lhs, const vector4& rhs)
{
	lhs /= rhs;
	return lhs;
}

vector4 operator*(vector4 lhs, const vector4& rhs)
{
	lhs *= rhs;
	return lhs;
}
