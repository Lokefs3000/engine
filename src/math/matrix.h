#pragma once

#include "vector3.h"
#include "quaternion.h"

__declspec(align(16)) struct matrix
{
private:
	__m128 m_value[4];
public:
	//constructors
	matrix();
	matrix(float scalar);
	matrix(float _00, float _01, float _02, float _03, float _10, float _11, float _12, float _13, float _20, float _21, float _22, float _23, float _30, float _31, float _32, float _33);

	//arithmetic
	void mul(matrix& a);
	matrix transpose();

	//statics
	matrix orthooffcenter(float left, float right, float bottom, float top, float near, float far);
	matrix ortho(float width, float height, float near, float far);
	matrix lookat(vector3 eye, vector3 at, vector3 up);
	matrix lookto(vector3 eye, vector3 dir, vector3 up);
	matrix transform(vector3 pos, quaternion rot, vector3 scale);
	matrix translation(vector3 pos);
	matrix scaling(vector3 scale);
	matrix rotation(quaternion rot);

	//getters
	float m00();
	float m01();
	float m02();
	float m03();
	float m10();
	float m11();
	float m12();
	float m13();
	float m20();
	float m21();
	float m22();
	float m23();
	float m30();
	float m31();
	float m32();
	float m33();

	//operators
};