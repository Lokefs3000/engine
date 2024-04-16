#include "../../src/pch.h"
#include "vector3.h"

#include "math.h"

vector3::vector3()
{
	vector3::vector3(0.0f, 0.0f, 0.0f);
}

vector3::vector3(float scalar)
{
	vector3::vector3(scalar, scalar, scalar);
}

vector3::vector3(float x, float y, float z)
{
	m_value = math::simd_create(x, y, z, 0.0f);
}

vector3::vector3(__m128 value)
{
	m_value = value;
}

void vector3::add(vector3& a)
{
	m_value = math::simd_add(m_value, a.m_value);
}

void vector3::sub(vector3& a)
{
	m_value = math::simd_sub(m_value, a.m_value);
}

void vector3::div(vector3& a)
{
	m_value = math::simd_div(m_value, a.m_value);
}

void vector3::mul(vector3& a)
{
	m_value = math::simd_mul(m_value, a.m_value);
}

vector3& vector3::normalize()
{
	__m128 res;

	__m128 vLengthSq = math::simd_mul(m_value, m_value);
	__m128 vTemp = math::simd_permuate(vLengthSq, 2, 1, 2, 1);
	vLengthSq = math::simd_add_ss(vLengthSq, vTemp);
	vTemp = math::simd_permuate(vTemp, 1, 1, 1, 1);
	vLengthSq = math::simd_add_ss(vLengthSq, vTemp);
	vLengthSq = math::simd_permuate(vLengthSq, 0, 0, 0, 0);

	res = math::simd_sqrt(vLengthSq);

	__m128 vZeroMask = math::simd_zero();

	vZeroMask = math::simd_compare(vZeroMask, res);

	vLengthSq = math::simd_compare(vLengthSq, { (float)0x7F800000, (float)0x7F800000, (float)0x7F800000, (float)0x7F800000 });

	res = math::simd_mul(m_value, res);

	res = math::simd_and(res, vZeroMask);

	__m128 vTemp1 = math::simd_andnot(vLengthSq, { (float)0x7FC00000, (float)0x7FC00000, (float)0x7FC00000, (float)0x7FC00000 });
	__m128 vTemp2 = math::simd_and(res, vLengthSq);

	m_value = math::simd_or(vTemp1, vTemp2);
	return *this;
}

vector3& vector3::cross(vector3& a)
{
	__m128 vTemp1 = math::simd_permuate(m_value, 3, 0, 2, 1);

	__m128 vTemp2 = math::simd_permuate(a.m_value, 3, 1, 0, 2);

	__m128 vResult = math::simd_mul(vTemp1, vTemp2);

	vTemp1 = math::simd_permuate(vTemp1, 3, 0, 2, 1);

	vTemp2 = math::simd_permuate(vTemp2, 3, 1, 0, 2);

	vResult = math::simd_sub(vTemp1, math::simd_mul(vTemp2, vResult));

	m_value = math::simd_add(vResult, { (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, 0x00000000 });
	return *this;
}

vector3& vector3::dot(vector3& a)
{
	__m128 vDot = math::simd_mul(m_value, a.m_value);

	__m128 vTemp = math::simd_permuate(vDot, 2, 1, 2, 1);

	vDot = math::simd_add_ss(vDot, vTemp);

	vTemp = math::simd_permuate(vTemp, 1, 1, 1, 1);

	vDot = math::simd_add_ss(vDot, vTemp);

	m_value = math::simd_permuate(vDot, 0, 0, 0, 0);
	return *this;
}

vector3 vector3::select(vector3 a, vector3 b, __m128 ctrl)
{
	__m128 tmp1 = math::simd_andnot(ctrl, a.m_value);
	__m128 tmp2 = math::simd_and(b.m_value, ctrl);
	return vector3(math::simd_or(tmp1, tmp2));
}

float vector3::x()
{
	return m_value.m128_f32[0];
}

float vector3::y()
{
	return m_value.m128_f32[1];
}

float vector3::z()
{
	return m_value.m128_f32[2];
}

__m128 vector3::value()
{
	return m_value;
}

vector3& vector3::operator+=(const vector3& rhs)
{
	m_value = math::simd_add(m_value, rhs.m_value);
	return *this;
}

vector3& vector3::operator-=(const vector3& rhs)
{
	m_value = math::simd_sub(m_value, rhs.m_value);
	return *this;
}

vector3& vector3::operator/=(const vector3& rhs)
{
	m_value = math::simd_div(m_value, rhs.m_value);
	return *this;
}

vector3& vector3::operator*=(const vector3& rhs)
{
	m_value = math::simd_mul(m_value, rhs.m_value);
	return *this;
}

vector3 operator+(vector3 lhs, const vector3& rhs)
{
	lhs += rhs;
	return lhs;
}

vector3 operator-(vector3 lhs, const vector3& rhs)
{
	lhs -= rhs;
	return lhs;
}

vector3 operator/(vector3 lhs, const vector3& rhs)
{
	lhs /= rhs;
	return lhs;
}

vector3 operator*(vector3 lhs, const vector3& rhs)
{
	lhs *= rhs;
	return lhs;
}

vector3 operator-(vector3 lhs)
{
	return vector3(-lhs.x(), -lhs.y(), -lhs.z());
}
