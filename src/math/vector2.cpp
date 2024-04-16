#include "../../src/pch.h"
#include "vector2.h"

#include "math.h"

vector2::vector2()
{
	vector2::vector2(0.0f, 0.0f);
}

vector2::vector2(float scalar)
{
	vector2::vector2(scalar, scalar);
}

vector2::vector2(float x, float y)
{
	m_value = math::simd_create(x, y, 0.0f, 0.0f);
}

void vector2::add(vector2& a)
{
	m_value = math::simd_add(m_value, a.m_value);
}

void vector2::sub(vector2& a)
{
	m_value = math::simd_sub(m_value, a.m_value);
}

void vector2::div(vector2& a)
{
	m_value = math::simd_div(m_value, a.m_value);
}

void vector2::mul(vector2& a)
{
	m_value = math::simd_mul(m_value, a.m_value);
}

float vector2::x()
{
	return m_value.m128_f32[0];
}

float vector2::y()
{
	return m_value.m128_f32[1];
}

vector2& vector2::operator+=(const vector2& rhs)
{
	m_value = math::simd_add(m_value, rhs.m_value);
	return *this;
}

vector2& vector2::operator-=(const vector2& rhs)
{
	m_value = math::simd_sub(m_value, rhs.m_value);
	return *this;
}

vector2& vector2::operator/=(const vector2& rhs)
{
	m_value = math::simd_div(m_value, rhs.m_value);
	return *this;
}

vector2& vector2::operator*=(const vector2& rhs)
{
	m_value = math::simd_mul(m_value, rhs.m_value);
	return *this;
}

vector2 operator+(vector2 lhs, const vector2& rhs)
{
	lhs += rhs;
	return lhs;
}

vector2 operator-(vector2 lhs, const vector2& rhs)
{
	lhs -= rhs;
	return lhs;
}

vector2 operator/(vector2 lhs, const vector2& rhs)
{
	lhs /= rhs;
	return lhs;
}

vector2 operator*(vector2 lhs, const vector2& rhs)
{
	lhs *= rhs;
	return lhs;
}
