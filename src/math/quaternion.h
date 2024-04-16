#pragma once

#include "vector4.h"

__declspec(align(16)) struct quaternion
{
private:
	__m128 m_value;
public:
	//constructors
	quaternion();
	quaternion(float x, float y, float z, float w);

	//arithmetic
	void mul(quaternion& a);

	//statics
	static quaternion euler(float rx, float ry, float rz);

	//getters
	float x();
	float y();
	float z();
	float w();
	__m128 value();

	//operators
};