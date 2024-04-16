#include "../../src/pch.h"
#include "quaternion.h"

#include "math.h"

quaternion::quaternion()
{
    quaternion::quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

quaternion::quaternion(float x, float y, float z, float w)
{
    m_value = math::simd_create(x, y, z, w);
}

void quaternion::mul(quaternion& a)
{
    static const __m128 ctrl_wzyx = __m128{ 1.0f, -1.0f, 1.0f, -1.0f };
    static const __m128 ctrl_zwxy = __m128{ 1.0f, 1.0f, -1.0f, -1.0f };
    static const __m128 ctrl_yxwz = __m128{ -1.0f, 1.0f, 1.0f, -1.0f };
    
    __m128 a2x = a.m_value;
    __m128 a2y = a.m_value;
    __m128 a2z = a.m_value;
    __m128 vres = a.m_value;
    //_MM_SHUFFLE
    vres = math::simd_permuate(vres, 3, 3, 3, 3);
    a2x = math::simd_permuate(a2x, 0, 0, 0, 0);
    a2y = math::simd_permuate(a2y, 1, 1, 1, 1);
    a2z = math::simd_permuate(a2z, 2, 2, 2, 2);

    vres = math::simd_mul(vres, m_value);
    __m128 shuffle = math::simd_permuate(m_value, 0, 1, 2, 3);

    a2x = math::simd_mul(a2x, shuffle);
    shuffle = math::simd_permuate(shuffle, 2, 3, 0, 1);

    vres = math::simd_add(math::simd_mul(a2x, ctrl_wzyx), vres);

    a2y = math::simd_mul(a2y, shuffle);
    shuffle = math::simd_permuate(shuffle, 0, 1, 2, 3);

    a2y = math::simd_mul(a2y, ctrl_zwxy);
    a2z = math::simd_mul(a2z, shuffle);

    a2y = math::simd_add(math::simd_mul(a2z, ctrl_yxwz), a2y);
    vres = math::simd_add(vres, a2y);

    m_value = vres;
}

quaternion quaternion::euler(float rx, float ry, float rz)
{
    return quaternion();
}

float quaternion::x()
{
    return m_value.m128_f32[0];
}

float quaternion::y()
{
    return m_value.m128_f32[1];
}

float quaternion::z()
{
    return m_value.m128_f32[2];
}

float quaternion::w()
{
    return m_value.m128_f32[3];
}
