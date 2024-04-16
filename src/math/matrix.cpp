#include "../../src/pch.h"
#include "matrix.h"

#include "math.h"
#include <DirectXMath.h>
matrix::matrix()
{
	matrix::matrix(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

matrix::matrix(float scalar)
{
	matrix::matrix(scalar, 0.0f, 0.0f, 0.0f, 0.0f, scalar, 0.0f, 0.0f, 0.0f, 0.0f, scalar, 0.0f, 0.0f, 0.0f, 0.0f, scalar);
}

matrix::matrix(float _00, float _01, float _02, float _03, float _10, float _11, float _12, float _13, float _20, float _21, float _22, float _23, float _30, float _31, float _32, float _33)
{
	m_value[0] = math::simd_create(_00, _01, _02, _03);
	m_value[1] = math::simd_create(_10, _11, _12, _13);
	m_value[2] = math::simd_create(_20, _21, _22, _23);
	m_value[3] = math::simd_create(_30, _31, _32, _33);
}

void matrix::mul(matrix& a)
{
	matrix res;

	//AVX PROBLEMS BROADCAST
	__m128 vw = m_value[0];
	__m128 vx = math::simd_permuate(vw, 0, 0, 0, 0);
	__m128 vy = math::simd_permuate(vw, 1, 1, 1, 1);
	__m128 vz = math::simd_permuate(vw, 2, 2, 2, 2);
	vw = math::simd_permuate(vw, 3, 3, 3, 3);

	vx = math::simd_mul(vx, a.m_value[0]);
	vy = math::simd_mul(vy, a.m_value[1]);
	vz = math::simd_mul(vz, a.m_value[2]);
	vw = math::simd_mul(vw, a.m_value[3]);

	vx = math::simd_add(vx, vz);
	vy = math::simd_add(vy, vw);
	vx = math::simd_add(vx, vy);
	res.m_value[0] = vx;

	//AVX PROBLEMS BROADCAST
	vw = m_value[1];
	vx = math::simd_permuate(vw, 0, 0, 0, 0);
	vy = math::simd_permuate(vw, 1, 1, 1, 1);
	vz = math::simd_permuate(vw, 2, 2, 2, 2);
	vw = math::simd_permuate(vw, 3, 3, 3, 3);

	vx = math::simd_mul(vx, a.m_value[0]);
	vy = math::simd_mul(vy, a.m_value[1]);
	vz = math::simd_mul(vz, a.m_value[2]);
	vw = math::simd_mul(vw, a.m_value[3]);

	vx = math::simd_add(vx, vz);
	vy = math::simd_add(vy, vw);
	vx = math::simd_add(vx, vy);
	res.m_value[1] = vx;

	//AVX PROBLEMS BROADCAST
	vw = m_value[2];
	vx = math::simd_permuate(vw, 0, 0, 0, 0);
	vy = math::simd_permuate(vw, 1, 1, 1, 1);
	vz = math::simd_permuate(vw, 2, 2, 2, 2);
	vw = math::simd_permuate(vw, 3, 3, 3, 3);

	vx = math::simd_mul(vx, a.m_value[0]);
	vy = math::simd_mul(vy, a.m_value[1]);
	vz = math::simd_mul(vz, a.m_value[2]);
	vw = math::simd_mul(vw, a.m_value[3]);

	vx = math::simd_add(vx, vz);
	vy = math::simd_add(vy, vw);
	vx = math::simd_add(vx, vy);
	res.m_value[2] = vx;

	//AVX PROBLEMS BROADCAST
	vw = m_value[3];
	vx = math::simd_permuate(vw, 0, 0, 0, 0);
	vy = math::simd_permuate(vw, 1, 1, 1, 1);
	vz = math::simd_permuate(vw, 2, 2, 2, 2);
	vw = math::simd_permuate(vw, 3, 3, 3, 3);

	vx = math::simd_mul(vx, a.m_value[0]);
	vy = math::simd_mul(vy, a.m_value[1]);
	vz = math::simd_mul(vz, a.m_value[2]);
	vw = math::simd_mul(vw, a.m_value[3]);

	vx = math::simd_add(vx, vz);
	vy = math::simd_add(vy, vw);
	vx = math::simd_add(vx, vy);
	res.m_value[3] = vx;

	m_value[0] = res.m_value[0];
	m_value[1] = res.m_value[1];
	m_value[2] = res.m_value[2];
	m_value[3] = res.m_value[3];
}

matrix matrix::transpose()
{
	__m128 vTemp1 = math::simd_shuffle(m_value[0], m_value[1], 1, 0, 1, 0);
	__m128 vTemp3 = math::simd_shuffle(m_value[0], m_value[1], 3, 2, 3, 2);
	__m128 vTemp2 = math::simd_shuffle(m_value[2], m_value[3], 1, 0, 1, 0);
	__m128 vTemp4 = math::simd_shuffle(m_value[2], m_value[3], 3, 2, 3, 2);

	m_value[0] = math::simd_shuffle(vTemp1, vTemp2, 2, 0, 2, 0);
	m_value[1] = math::simd_shuffle(vTemp1, vTemp2, 3, 1, 3, 1);
	m_value[2] = math::simd_shuffle(vTemp3, vTemp4, 2, 0, 2, 0);
	m_value[3] = math::simd_shuffle(vTemp3, vTemp4, 3, 1, 3, 1);
	return *this;
}

matrix matrix::orthooffcenter(float left, float right, float bottom, float top, float near, float far)
{
	matrix m;
	float freciW = 1.0f / (right - left);
	float freciH = 1.0f / (top - bottom);
	float frange = 1.0f / (far - near);

	__m128 rmem1 = math::simd_create(freciW, freciH, frange, 1.0f);
	__m128 rmem2 = math::simd_create((-left + right), -(top + bottom), -near, 1.0f);

	__m128 vals = rmem1;
	__m128 temp = math::simd_zero();

	temp = math::simd_move_ss(temp, vals);
	temp = math::simd_add_ss(temp, temp);
	m.m_value[0] = temp;

	temp = vals;
	temp = math::simd_and(temp, { (float)0x00000000, (float)0xFFFFFFFF, (float)0x00000000, (float)0x00000000 });
	temp = math::simd_add(temp, temp);
	m.m_value[1] = temp;

	temp = vals;
	temp = math::simd_and(temp, { (float)0x00000000, (float)0xFFFFFFFF, (float)0x00000000, (float)0x00000000 });
	m.m_value[2] = temp;

	vals = math::simd_mul(vals, rmem2);
	m.m_value[3] = vals;

	return m;
}

matrix matrix::ortho(float width, float height, float far, float near)
{
	matrix m;
	float frange = 1.0f / (far - near);

	__m128 rmem = {
		2.0f / width,
		2.0f / height,
		frange,
		-frange * near
	};

	__m128 vals = rmem;
	__m128 temp = math::simd_zero();

	temp = math::simd_move_ss(temp, vals);

	m.m_value[0] = temp;

	temp = vals;
	temp = math::simd_and(temp, { (float)0x00000000, (float)0xFFFFFFFF, (float)0x00000000, (float)0x00000000 });
	m.m_value[1] = temp;

	temp = math::simd_zero();
	vals = math::simd_shuffle(vals, { 0.0f, 0.0f, 0.0f, 1.0f }, 3, 2, 3, 2);

	temp = math::simd_shuffle(temp, vals, 2, 0, 0, 0);
	m.m_value[2] = temp;

	temp = math::simd_shuffle(temp, vals, 3, 1, 0, 0);
	m.m_value[3] = temp;
	
	return m;
}

matrix matrix::lookat(vector3 eye, vector3 at, vector3 up)
{
	vector3 eyedir = at - eye;
	return lookto(eye, eyedir, up);
}

matrix matrix::lookto(vector3 eye, vector3 dir, vector3 up)
{
	vector3 r2 = dir.normalize();
	vector3 r0 = up.cross(r2).normalize();
	vector3 r1 = r2.cross(r0);

	vector3 neye = -eye;

	vector3 d0 = r0.dot(neye);
	vector3 d1 = r1.dot(neye);
	vector3 d2 = r2.dot(neye);

	matrix m;
	m.m_value[0] = vector3::select(d0, r0, { (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 }).value();
	m.m_value[1] = vector3::select(d1, r1, { (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 }).value();
	m.m_value[2] = vector3::select(d2, r2, { (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 }).value();
	m.m_value[3] = { 0.0f, 0.0f, 0.0f, 1.0f };

	return m.transpose();
}

matrix matrix::transform(vector3 pos, quaternion rot, vector3 scale)
{
	vector3 VScalingOrigin = vector3::select(vector3(__m128{ (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 }), vector3(0.0f), __m128{ (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 });
	vector3 NegScalingOrigin = vector3(0.0f);

	matrix MScalingOriginI = matrix::translation(NegScalingOrigin);
	matrix MScalingOrientation = matrix::rotation(quaternion());
	matrix MScalingOrientationT = MScalingOrientation.transpose();
	matrix MScaling = matrix::scaling(scale);
	vector3 VRotationOrigin = vector3::select(vector3(__m128{ (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 }), vector3(), __m128{ (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 });
	matrix MRotation = matrix::rotation(rot);
	vector3 VTranslation = vector3::select(vector3(__m128{ (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 }), pos, __m128{ (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 });

	matrix M;
	MScalingOriginI.mul(MScalingOrientationT);
	M.mul(MScaling);
	M.mul(MScalingOrientation);
	M.m_value[3] = (vector3(M.m_value[3]) - VScalingOrigin).value();
	M.m_value[3] = (vector3(M.m_value[3]) - VRotationOrigin).value();
	M.mul(MRotation);
	M.m_value[3] = (vector3(M.m_value[3]) + VRotationOrigin).value();
	M.m_value[3] = (vector3(M.m_value[3]) + VTranslation).value();
	return M;
}

matrix matrix::translation(vector3 pos)
{
	matrix M;
	M.m_value[3] = math::simd_create(pos.x(), pos.y(), pos.z(), 1.0f);
	return M;
}

matrix matrix::scaling(vector3 scale)
{
	matrix M;
	M.m_value[1] = math::simd_create(scale.x(), 0.0f, 0.0f, 0.0f);
	M.m_value[2] = math::simd_create(0.0f, scale.y(), 0.0f, 0.0f);
	M.m_value[3] = math::simd_create(0.0f, 0.0f, scale.z(), 0.0f);
	return M;
}

matrix matrix::rotation(quaternion rot)
{
	static const __m128 Constant1110 = { 1.0f, 1.0f, 1.0f, 0.0f };

	__m128 Q0 = math::simd_add(rot.value(), rot.value());
	__m128 Q1 = math::simd_mul(rot.value(), Q0);

	__m128 V0 = math::simd_permuate(Q1, 3, 0, 0, 1);
	V0 = math::simd_and(V0, { (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 });
	__m128 V1 = math::simd_permuate(Q1, 3, 1, 2, 2);
	V1 = math::simd_and(V1, { (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 });
	__m128 R0 = _mm_sub_ps(Constant1110, V0);
	R0 = math::simd_sub(R0, V1);

	V0 = math::simd_permuate(rot.value(), 3, 1, 0, 0);
	V1 = math::simd_permuate(Q0, 3, 2, 1, 2);
	V0 = math::simd_mul(V0, V1);

	V1 = math::simd_permuate(rot.value(), 3, 3, 3, 3);
	__m128 V2 = math::simd_permuate(Q0,3, 0, 2, 1);
	V1 = math::simd_mul(V1, V2);

	__m128 R1 = math::simd_add(V0, V1);
	__m128 R2 = math::simd_sub(V0, V1);

	V0 = math::simd_shuffle(R1, R2, 1, 0, 2, 1);
	V0 = math::simd_permuate(V0, 1, 3, 2, 0);
	V1 = math::simd_shuffle(R1, R2, 2, 2, 0, 0);
	V1 = math::simd_permuate(V1, 2, 0, 2, 0);

	Q1 = math::simd_shuffle(R0, V0, 1, 0, 3, 0);
	Q1 = math::simd_permuate(Q1, 1, 3, 2, 0);

	matrix M;
	M.m_value[0] = Q1;

	Q1 = math::simd_shuffle(R0, V0, 3, 2, 3, 1);
	Q1 = math::simd_permuate(Q1, 1, 3, 0, 2);
	M.m_value[1] = Q1;

	Q1 = math::simd_shuffle(V1, R0, 3, 2, 1, 0);
	M.m_value[2] = Q1;
	M.m_value[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
	return M;
}
