#pragma once

#include <immintrin.h>

__declspec(align(16)) struct vector3
{
private:
	__m128 m_value;
public:
	//constructors
	vector3();
	vector3(float scalar);
	vector3(float x, float y, float z);
	vector3(__m128 value);

	//arithmetic
	void add(vector3& a);
	void sub(vector3& a);
	void div(vector3& a);
	void mul(vector3& a);

	//math
	vector3& normalize();
	vector3& cross(vector3& a);
	vector3& dot(vector3& a);

	//statics
	static vector3 select(vector3 a, vector3 b, __m128 ctrl);

	//getters
	float x();
	float y();
	float z();
	__m128 value();

	//operators
	vector3& operator+=(const vector3& rhs);
	friend vector3 operator+(vector3 lhs, const vector3& rhs);
	vector3& operator-=(const vector3& rhs);
	friend vector3 operator-(vector3 lhs, const vector3& rhs);
	vector3& operator/=(const vector3& rhs);
	friend vector3 operator/(vector3 lhs, const vector3& rhs);
	vector3& operator*=(const vector3& rhs);
	friend vector3 operator*(vector3 lhs, const vector3& rhs);
	
	friend vector3 operator-(vector3 lhs);
};