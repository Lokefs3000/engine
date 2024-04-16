#pragma once

#include <immintrin.h>

__declspec(align(16)) struct vector2
{
private:
	__m128 m_value;
public:
	//constructors
	vector2();
	vector2(float scalar);
	vector2(float x, float y);

	//arithmetic
	void add(vector2& a);
	void sub(vector2& a);
	void div(vector2& a);
	void mul(vector2& a);

	//getters
	float x();
	float y();

	//operators
	vector2& operator+=(const vector2& rhs);
	friend vector2 operator+(vector2 lhs, const vector2& rhs);
	vector2& operator-=(const vector2& rhs);
	friend vector2 operator-(vector2 lhs, const vector2& rhs);
	vector2& operator/=(const vector2& rhs);
	friend vector2 operator/(vector2 lhs, const vector2& rhs);
	vector2& operator*=(const vector2& rhs);
	friend vector2 operator*(vector2 lhs, const vector2& rhs);
};