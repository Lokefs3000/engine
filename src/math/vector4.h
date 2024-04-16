#pragma once

#include <immintrin.h>

__declspec(align(16)) struct vector4
{
private:
	__m128 m_value;
public:
	//constructors
	vector4();
	vector4(float scalar);
	vector4(float x, float y, float z, float w);

	//arithmetic
	void add(vector4& a);
	void sub(vector4& a);
	void div(vector4& a);
	void mul(vector4& a);

	//getters
	float x();
	float y();
	float z();
	float w();

	//operators
	vector4& operator+=(const vector4& rhs);
	friend vector4 operator+(vector4 lhs, const vector4& rhs);
	vector4& operator-=(const vector4& rhs);
	friend vector4 operator-(vector4 lhs, const vector4& rhs);
	vector4& operator/=(const vector4& rhs);
	friend vector4 operator/(vector4 lhs, const vector4& rhs);
	vector4& operator*=(const vector4& rhs);
	friend vector4 operator*(vector4 lhs, const vector4& rhs);
};

__declspec(align(16)) struct vector4f
{
	float x, y, z, w;
};