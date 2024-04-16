#include "../../src/pch.h"
#include "math.h"

#include <cstdint>
#include <intrin.h>

#include <spdlog/spdlog.h>
#define warn(...) spdlog::get("ENGINE")->warn(__VA_ARGS__)

class simd_math
{
public:
	virtual __m128 make(float x, float y, float z, float w) = 0;
	virtual __m128 zero() = 0;

	virtual __m128 add(__m128 a, __m128 b) = 0;
	virtual __m128 sub(__m128 a, __m128 b) = 0;
	virtual __m128 div(__m128 a, __m128 b) = 0;
	virtual __m128 mul(__m128 a, __m128 b) = 0;

	virtual __m128 v_select(__m128 a, __m128 b, __m128 s) = 0;
	virtual __m128 v_permuate(int x, int y, int z, int w, __m128 a, __m128 b) = 0;
	virtual __m128 v_sincos(__m128& a, __m128& b, __m128 c) = 0;
	virtual __m128 v_modangles(__m128 a) = 0;
	virtual __m128 v_round(__m128 a) = 0;

	virtual __m128 v2_norm(__m128 ab) = 0;
	virtual __m128 v2_dot(__m128 a, __m128 b) = 0;
	virtual __m128 v2_cross(__m128 a, __m128 b) = 0;

	virtual __m128 v3_norm(__m128 a) = 0;
	virtual __m128 v3_dot(__m128 a, __m128 b) = 0;
	virtual __m128 v3_cross(__m128 a, __m128 b) = 0;

	virtual __m128 v4_norm(__m128 a) = 0;
	virtual __m128 v4_dot(__m128 a, __m128 b) = 0;
	virtual __m128 v4_cross(__m128 a, __m128 b, __m128 c) = 0;

	virtual __m128 qt_mul(__m128 a, __m128 b) = 0;
	virtual __m128 qt_euler(float rx, float ry, float rz) = 0;
	virtual __m128 qt_rpyv(__m128 angles) = 0;

	virtual __m128* mt_mul(__m128 a[4], __m128 b[4]) = 0;
	virtual __m128* mt_transpose(__m128 a[4]) = 0;
	virtual __m128* mt_orthooffcenter(float left, float right, float bottom, float top, float near, float far) = 0;
	virtual __m128* mt_ortho(float width, float height, float near, float far) = 0;
	virtual __m128* mt_lookat(__m128 eye, __m128 at, __m128 up) = 0;
	virtual __m128* mt_lookto(__m128 eye, __m128 dir, __m128 up) = 0;
	virtual __m128* mt_transform(__m128 pos, __m128 rot, __m128 scale) = 0;
	virtual __m128* mt_translation(__m128 pos) = 0;
	virtual __m128* mt_scaling(__m128 scale) = 0;
	virtual __m128* mt_rotation(__m128 rot) = 0;
};

#define shuffle(fp3, fp2, fp1, fp0) (((fp3) << 6) | ((fp2) << 4) | \
                                     ((fp1) << 2) | ((fp0)))

static const float pi = 0.0f;
static const float pidiv2 = 0.0f;
static const float twopi = 0.0f;
static const float onediv2pi = 0.0f;

static const __m128 s_infinity = { 0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000 };
static const __m128 s_nan = { 0x7FC00000, 0x7FC00000, 0x7FC00000, 0x7FC00000 };
static const __m128 s_negzero = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
static const __m128 s_mask3 = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 };
static const __m128 s_halfpi = { pidiv2, pidiv2, pidiv2, pidiv2 };
static const __m128 s_pi = { pi, pi, pi, pi };
static const __m128 s_one = { 1.0f, 1.0f, 1.0f, 1.0f };
static const __m128 s_negone = { -1.0f, -1.0f, -1.0f, -1.0f };
static const __m128 s_twopi = { twopi, twopi, twopi, twopi };
static const __m128 s_sincoeff1 = { -2.3889859e-08f, -0.16665852f, +0.0083139502f, -0.00018524670f };
static const __m128 s_sincoeff0 = { -0.16666667f, +0.0083333310f, -0.00019840874f, +2.7525562e-06f };
static const __m128 s_coscoeff1 = { -2.6051615e-07f, -0.49992746f, +0.041493919f, -0.0012712436f };
static const __m128 s_coscoeff0 = { -0.5f, +0.041666638f, -0.0013888378f, +2.4760495e-05f };
static const __m128 s_rectwopi = { onediv2pi, onediv2pi, onediv2pi, onediv2pi };
static const __m128 s_onehalf = { 0.5f, 0.5f, 0.5f, 0.5f };
static const __m128 s_nofrac = { 8388608.0f, 8388608.0f, 8388608.0f, 8388608.0f };
static const __m128 s_absmask = { 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF };

class simd_sse2 : public simd_math
{
public:
	__m128 make(float x, float y, float z, float w) override
	{
		return _mm_set_ps(w, z, y, x);
	}
	__m128 zero() override
	{
		return _mm_setzero_ps();
	}

	__m128 add(__m128 a, __m128 b) override
	{
		return _mm_add_ps(a, b);
	}
	__m128 sub(__m128 a, __m128 b) override
	{
		return _mm_sub_ps(a, b);
	}
	__m128 div(__m128 a, __m128 b) override
	{
		return _mm_div_ps(a, b);
	}
	__m128 mul(__m128 a, __m128 b) override
	{
		return _mm_mul_ps(a, b);
	}

	__m128 v_select(__m128 a, __m128 b, __m128 s) override
	{
		__m128 temp1 = _mm_andnot_ps(s, a);
		__m128 temp2 = _mm_and_ps(b, s);
		return _mm_or_ps(temp1, temp2);
	}

	__m128 v_permuate(int x, int y, int z, int w, __m128 a, __m128 b) override
	{
		bool wx = x > 3;
		bool wy = y > 3;
		bool wz = z > 3;
		bool ww = w > 3;

		__m128 select = { wx ? 0xFFFFFFFF : 0, wy ? 0xFFFFFFFF : 0, wz ? 0xFFFFFFFF : 0, ww ? 0xFFFFFFFF : 0 };

		__m128 shuffled1 = _mm_shuffle_ps(a, a, shuffle(w & 3, z & 3, y & 3, x & 3));
		__m128 shuffled2 = _mm_shuffle_ps(b, b, shuffle(w & 3, z & 3, y & 3, x & 3));
	
		__m128 masked1 = _mm_andnot_ps(select, shuffled1);
		__m128 masked2 = _mm_andnot_ps(select, shuffled2);

		return _mm_or_ps(masked1, masked2);
	}

	__m128 v_sincos(__m128& a, __m128& b, __m128 c) override
	{
		__m128 x = v_modangles(c);

		__m128 sign = _mm_and_ps(x, s_negzero);
		__m128 c = _mm_or_ps(s_pi, sign);
		__m128 absx = _mm_andnot_ps(sign, x);
		__m128 rflx = _mm_sub_ps(c, x);
		__m128 comp = _mm_cmple_ps(absx, s_halfpi);
		__m128 sel0 = _mm_and_ps(comp, x);
		__m128 sel1 = _mm_andnot_ps(comp, rflx);
		x = _mm_or_ps(sel0, sel1);
		sel0 = _mm_and_ps(comp, s_one);
		sel1 = _mm_andnot_ps(comp, s_negone);
		sign = _mm_or_ps(sel0, sel1);

		__m128 x2 = _mm_mul_ps(x, x);

		const __m128 sc1 = s_sincoeff1;
		__m128 vconstsb = _mm_shuffle_ps(sc1, sc1, shuffle(0, 0, 0, 0));
		const __m128 sc0 = s_sincoeff0;
		__m128 vconsts = _mm_shuffle_ps(sc0, sc0, shuffle(3, 3, 3, 3));
		__m128 res = _mm_sub_ps(vconstsb, _mm_mul_ps(x2, vconsts));

		vconsts = _mm_shuffle_ps(sc0, sc0, shuffle(2, 2, 2, 2));
		res = _mm_sub_ps(res, _mm_mul_ps(x2, vconsts));

		vconsts = _mm_shuffle_ps(sc0, sc0, shuffle(1, 1, 1, 1));
		res = _mm_sub_ps(res, _mm_mul_ps(x2, vconsts));

		vconsts = _mm_shuffle_ps(sc0, sc0, shuffle(0, 0, 0, 0));
		res = _mm_sub_ps(res, _mm_mul_ps(x2, vconsts));

		res = _mm_sub_ps(res, _mm_mul_ps(x2, s_one));
		res = _mm_mul_ps(res, x);
		a = res;

		const __m128 cc1 = s_coscoeff1;
		vconstsb = _mm_shuffle_ps(cc1, cc1, shuffle(0, 0, 0, 0));
		const __m128 cc0 = s_coscoeff0;
		vconsts = _mm_shuffle_ps(cc0, cc0, shuffle(3, 3, 3, 3));
		res = _mm_sub_ps(vconstsb, _mm_mul_ps(x2, vconsts));;

		vconsts = _mm_shuffle_ps(cc0, cc0, shuffle(2, 2, 2, 2));
		res = _mm_sub_ps(res, _mm_mul_ps(x2, vconsts));

		vconsts = _mm_shuffle_ps(cc0, cc0, shuffle(1, 1, 1, 1));
		res = _mm_sub_ps(res, _mm_mul_ps(x2, vconsts));

		vconsts = _mm_shuffle_ps(cc0, cc0, shuffle(0, 0, 0, 0));
		res = _mm_sub_ps(res, _mm_mul_ps(x2, vconsts));

		res = _mm_sub_ps(res, _mm_mul_ps(x2, s_one));
		res = _mm_mul_ps(res, sign);
		b = res;
	}

	__m128 v2_norm(__m128 ab) override
	{
		__m128 lensq = _mm_mul_ps(ab, ab);
		__m128 temp = _mm_shuffle_ps(lensq, lensq, shuffle(1, 1, 1, 1));
		lensq = _mm_add_ss(lensq, temp);
		lensq = _mm_shuffle_ps(lensq, lensq, shuffle(0, 0, 0, 0));

		__m128 res = _mm_sqrt_ps(lensq);

		__m128 zeromask = _mm_setzero_ps();
		zeromask = _mm_cmpneq_ps(zeromask, res);

		lensq = _mm_cmpneq_ps(lensq, s_infinity);
		
		res = _mm_div_ps(ab, res);
		res = _mm_and_ps(res, res);

		__m128 temp1 = _mm_andnot_ps(lensq, s_nan);
		__m128 temp2 = _mm_and_ps(res, lensq);
		res = _mm_or_ps(temp1, temp2);
		return res;
	}
	__m128 v2_dot(__m128 a, __m128 b) override
	{
		__m128 lensq = _mm_mul_ps(a, b);
		__m128 temp = _mm_shuffle_ps(lensq, lensq, shuffle(1, 1, 1, 1));
	
		lensq = _mm_add_ss(lensq, temp);
		lensq = _mm_shuffle_ps(lensq, lensq, shuffle(0, 0, 0, 0));
		return lensq;
	}
	__m128 v2_cross(__m128 a, __m128 b) override
	{
		__m128 res = _mm_shuffle_ps(b, b, shuffle(0, 1, 0, 1));
		res = _mm_mul_ps(res, a);

		__m128 temp = _mm_shuffle_ps(res, res, shuffle(1, 1, 1, 1));
		res = _mm_sub_ss(res, temp);

		res = _mm_shuffle_ps(res, res, shuffle(0, 0, 0, 0));
		return res;
	}

	__m128 v3_norm(__m128 a) override
	{
		__m128 lensq = _mm_mul_ps(a, a);
		__m128 temp = _mm_shuffle_ps(lensq, lensq, shuffle(2, 1, 2, 1));

		lensq = _mm_add_ss(lensq, temp);
		temp = _mm_shuffle_ps(temp, temp, shuffle(1, 1, 1, 1));
		lensq = _mm_add_ss(lensq, temp);
		lensq = _mm_shuffle_ps(lensq, lensq, shuffle(0, 0, 0, 0));

		__m128 res = _mm_sqrt_ps(lensq);
		__m128 zeromask = _mm_setzero_ps();

		zeromask = _mm_cmpneq_ps(zeromask, res);
		lensq = _mm_cmpneq_ps(lensq, s_infinity);
		
		res = _mm_div_ps(a, res);
		res = _mm_and_ps(res, zeromask);

		__m128 temp1 = _mm_andnot_ps(lensq, s_nan);
		__m128 temp2 = _mm_and_ps(res, lensq);
		res = _mm_or_ps(temp1, temp2);
		return res;
	}

	__m128 v_modangles(__m128 a) override
	{
		__m128 res = _mm_mul_ps(a, s_rectwopi);
		res = v_round(res);
		return _mm_sub_ps(res, _mm_mul_ps(s_twopi, a));
	}

	__m128 v_round(__m128 a) override
	{
		__m128 sign = _mm_and_ps(a, s_negzero);
		__m128 smagic = _mm_or_ps(s_nofrac, sign);
		__m128 r1 = _mm_add_ps(a, smagic);
		r1 = _mm_sub_ps(r1, smagic);
		__m128 r2 = _mm_and_ps(a, s_absmask);
		__m128 mask = _mm_cmple_ps(r2, s_nofrac);
		r2 = _mm_andnot_ps(mask, a);
		r1 = _mm_and_ps(r1, mask);
		__m128 res = _mm_xor_ps(r1, r2);
		return res;
	}

	__m128 v3_dot(__m128 a, __m128 b) override
	{
		__m128 dot = _mm_mul_ps(a, b);
		__m128 temp = _mm_shuffle_ps(dot, dot, shuffle(2, 1, 2, 1));
		
		dot = _mm_add_ss(dot, temp);
		dot = _mm_shuffle_ps(temp, temp, shuffle(1, 1, 1, 1));
		dot = _mm_add_ss(dot, temp);

		return _mm_shuffle_ps(dot, dot, shuffle(0, 0, 0, 0));
	}
	__m128 v3_cross(__m128 a, __m128 b) override
	{
		__m128 temp1 = _mm_shuffle_ps(a, a, shuffle(3, 0, 2, 1));
		__m128 temp2 = _mm_shuffle_ps(b, b, shuffle(3, 1, 0, 2));
		__m128 res = _mm_mul_ps(temp1, temp2);

		temp1 = _mm_shuffle_ps(temp1, temp1, shuffle(3, 0, 2, 1));
		temp2 = _mm_shuffle_ps(temp2, temp2, shuffle(3, 1, 0, 2));
		res = _mm_sub_ps(res, _mm_mul_ps(temp1, temp2));
		
		return _mm_and_ps(res, s_mask3);
	}

	__m128 v4_norm(__m128 a)override
	{

	}
	__m128 v4_dot(__m128 a, __m128 b) override
	{
		__m128 temp2 = b;
		__m128 temp = _mm_mul_ps(a, temp2);

		temp2 = _mm_shuffle_ps(temp2, temp, shuffle(1, 0, 0, 0));
		temp2 = _mm_add_ps(temp2, temp);
		temp = _mm_shuffle_ps(temp, temp2, shuffle(0, 3, 0, 0));
		temp = _mm_add_ps(temp, temp2);

		return _mm_shuffle_ps(temp, temp, shuffle(2, 2, 2, 2));
	}
	__m128 v4_cross(__m128 a, __m128 b, __m128 c) override
	{
		__m128 res = _mm_shuffle_ps(b, b, shuffle(2, 1, 3, 2));
		__m128 temp3 = _mm_shuffle_ps(c, c, shuffle(1, 3, 2, 1));
		res = _mm_mul_ps(res, temp3);

		__m128 temp2 = _mm_shuffle_ps(b, b, shuffle(1, 3, 2, 3));
		temp3 = _mm_shuffle_ps(temp3, temp3, shuffle(1, 3, 0, 1));
		res = _mm_sub_ps(temp2, _mm_mul_ps(temp3, res));

		__m128 temp1 = _mm_shuffle_ps(a, a, shuffle(0, 0, 0, 1));
		res = _mm_mul_ps(res, temp1);

		temp2 = _mm_shuffle_ps(b, b, shuffle(2, 0, 3, 1));
		temp3 = _mm_shuffle_ps(c, c, shuffle(0, 3, 0, 3));
		temp3 = _mm_mul_ps(temp3, temp2);

		temp2 = _mm_shuffle_ps(temp2, temp2, shuffle(2, 1, 2, 1));
		temp1 = _mm_shuffle_ps(c, c, shuffle(2, 0, 3, 1));
		temp3 = _mm_sub_ps(temp2, _mm_mul_ps(temp1, temp3));

		temp1 = _mm_shuffle_ps(a, a, shuffle(1, 1, 2, 2));
		res = _mm_sub_ps(temp1, _mm_mul_ps(temp3, res));

		temp2 = _mm_shuffle_ps(b, b, shuffle(1, 0, 2, 1));
		temp3 = _mm_shuffle_ps(c, c, shuffle(0, 1, 0, 2));
		temp3 = _mm_mul_ps(temp3, temp2);

		temp2 = _mm_shuffle_ps(temp2, temp2, shuffle(2, 0, 2, 1));
		temp1 = _mm_shuffle_ps(c, c, shuffle(1, 0, 2, 1));
		temp3 = _mm_sub_ps(temp1, _mm_mul_ps(temp2, temp3));

		temp1 = _mm_shuffle_ps(a, a, shuffle(2, 3, 3, 3));
		res = _mm_sub_ps(temp3, _mm_mul_ps(temp1, res));
		return res;
	}

	__m128 qt_mul(__m128 a, __m128 b) override
	{
		static const __m128 ctrl_wzyx = __m128{ 1.0f, -1.0f, 1.0f, -1.0f };
		static const __m128 ctrl_zwxy = __m128{ 1.0f, 1.0f, -1.0f, -1.0f };
		static const __m128 ctrl_yxwz = __m128{ -1.0f, 1.0f, 1.0f, -1.0f };

		__m128 a2x = b;
		__m128 a2y = b;
		__m128 a2z = b;
		__m128 vres = b;
		//_MM_SHUFFLE
		vres = _mm_shuffle_ps(vres, vres, shuffle(3, 3, 3, 3));
		a2x = _mm_shuffle_ps(a2x, a2x, shuffle(0, 0, 0, 0));
		a2y = _mm_shuffle_ps(a2y, a2y, shuffle(1, 1, 1, 1));
		a2z = _mm_shuffle_ps(a2z, a2z, shuffle(2, 2, 2, 2));

		vres = _mm_mul_ps(vres, a);
		__m128 shuffle = _mm_shuffle_ps(a, a, shuffle(0, 1, 2, 3));

		a2x = _mm_mul_ps(a2x, shuffle);
		shuffle = _mm_shuffle_ps(shuffle, shuffle, shuffle(2, 3, 0, 1));

		vres = _mm_add_ps(_mm_mul_ps(a2x, ctrl_wzyx), vres);

		a2y = _mm_mul_ps(a2y, shuffle);
		shuffle = _mm_shuffle_ps(shuffle, shuffle, shuffle(0, 1, 2, 3));

		a2y = _mm_mul_ps(a2y, ctrl_zwxy);
		a2z = _mm_mul_ps(a2z, shuffle);

		a2y = _mm_add_ps(_mm_mul_ps(a2z, ctrl_yxwz), a2y);
		vres = _mm_add_ps(vres, a2y);
		return vres;
	}
	__m128 qt_euler(float rx, float ry, float rz) override
	{
		__m128 angles = _mm_set_ps(rx, ry, rz, 0.0f);
		return qt_rpyv(angles);
	}
	__m128 qt_rpyv(__m128 angles) override
	{
		static const __m128 sign = { 1.0f, -1.0f, -1.0f, 1.0f };

		__m128 half = _mm_mul_ps(angles, s_onehalf);

		__m128 sin, cos;
		v_sincos(sin, cos, half);

		__m128 p0 = v_permuate(0, 1, 1, 1, sin, cos);
		__m128 y0 = v_permuate(5, 1, 5, 5, sin, cos);
		__m128 r0 = v_permuate(6, 6, 2, 6, sin, cos);
		__m128 p1 = v_permuate(0, 1, 1, 1, cos, sin);
		__m128 y1 = v_permuate(5, 1, 5, 5, cos, sin);
		__m128 r1 = v_permuate(6, 6, 2, 6, cos, sin);

		__m128 q1 = _mm_mul_ps(p1, sign);
		__m128 q0 = _mm_mul_ps(p0, y0);
		q1 = _mm_mul_ps(q1, y1);
		q0 = _mm_mul_ps(q0, r0);
		__m128 q = _mm_add_ps(_mm_mul_ps(q1, r1), q0);
		return q;
	}

	__m128* mt_mul(__m128 a[4], __m128 b[4]) override
	{
		__m128 res[4];

		//AVX PROBLEMS BROADCAST
		__m128 vw = a[0];
		__m128 vx = _mm_shuffle_ps(vw, vw, shuffle(0, 0, 0, 0));
		__m128 vy = _mm_shuffle_ps(vw, vw, shuffle(1, 1, 1, 1));
		__m128 vz = _mm_shuffle_ps(vw, vw, shuffle(2, 2, 2, 2));
		vw = _mm_shuffle_ps(vw, vw, shuffle(3, 3, 3, 3));

		vx = _mm_mul_ps(vx, b[0]);
		vy = _mm_mul_ps(vy, b[1]);
		vz = _mm_mul_ps(vz, b[2]);
		vw = _mm_mul_ps(vw, b[3]);

		vx = _mm_add_ps(vx, vz);
		vy = _mm_add_ps(vy, vw);
		vx = _mm_add_ps(vx, vy);
		res[0] = vx;

		//AVX PROBLEMS BROADCAST
		vw = a[1];
		vx = _mm_shuffle_ps(vw, vw, shuffle(0, 0, 0, 0));
		vy = _mm_shuffle_ps(vw, vw, shuffle(1, 1, 1, 1));
		vz = _mm_shuffle_ps(vw, vw, shuffle(2, 2, 2, 2));
		vw = _mm_shuffle_ps(vw, vw, shuffle(3, 3, 3, 3));

		vx = _mm_mul_ps(vx, b[0]);
		vy = _mm_mul_ps(vy, b[1]);
		vz = _mm_mul_ps(vz, b[2]);
		vw = _mm_mul_ps(vw, b[3]);

		vx = _mm_add_ps(vx, vz);
		vy = _mm_add_ps(vy, vw);
		vx = _mm_add_ps(vx, vy);
		res[1] = vx;

		//AVX PROBLEMS BROADCAST
		vw = a[2];
		vx = _mm_shuffle_ps(vw, vw, shuffle(0, 0, 0, 0));
		vy = _mm_shuffle_ps(vw, vw, shuffle(1, 1, 1, 1));
		vz = _mm_shuffle_ps(vw, vw, shuffle(2, 2, 2, 2));
		vw = _mm_shuffle_ps(vw, vw, shuffle(3, 3, 3, 3));

		vx = _mm_mul_ps(vx, b[0]);
		vy = _mm_mul_ps(vy, b[1]);
		vz = _mm_mul_ps(vz, b[2]);
		vw = _mm_mul_ps(vw, b[3]);

		vx = _mm_add_ps(vx, vz);
		vy = _mm_add_ps(vy, vw);
		vx = _mm_add_ps(vx, vy);
		res[2] = vx;

		//AVX PROBLEMS BROADCAST
		vw = a[3];
		vx = _mm_shuffle_ps(vw, vw, shuffle(0, 0, 0, 0));
		vy = _mm_shuffle_ps(vw, vw, shuffle(1, 1, 1, 1));
		vz = _mm_shuffle_ps(vw, vw, shuffle(2, 2, 2, 2));
		vw = _mm_shuffle_ps(vw, vw, shuffle(3, 3, 3, 3));

		vx = _mm_mul_ps(vx, b[0]);
		vy = _mm_mul_ps(vy, b[1]);
		vz = _mm_mul_ps(vz, b[2]);
		vw = _mm_mul_ps(vw, b[3]);

		vx = _mm_add_ps(vx, vz);
		vy = _mm_add_ps(vy, vw);
		vx = _mm_add_ps(vx, vy);
		res[3] = vx;

		return res;
	}
	__m128* mt_transpose(__m128 a[4]) override
	{
		__m128 vTemp1 = _mm_shuffle_ps(a[0], a[1], shuffle(1, 0, 1, 0));
		__m128 vTemp3 = _mm_shuffle_ps(a[0], a[1], shuffle(3, 2, 3, 2));
		__m128 vTemp2 = _mm_shuffle_ps(a[2], a[3], shuffle(1, 0, 1, 0));
		__m128 vTemp4 = _mm_shuffle_ps(a[2], a[3], shuffle(3, 2, 3, 2));

		a[0] = _mm_shuffle_ps(vTemp1, vTemp2, shuffle(2, 0, 2, 0));
		a[1] = _mm_shuffle_ps(vTemp1, vTemp2, shuffle(3, 1, 3, 1));
		a[2] = _mm_shuffle_ps(vTemp3, vTemp4, shuffle(2, 0, 2, 0));
		a[3] = _mm_shuffle_ps(vTemp3, vTemp4, shuffle(3, 1, 3, 1));
		return a;
	}
#undef far //win32 troubles
#undef near //same ^^^^^^^^
	__m128* mt_orthooffcenter(float left, float right, float bottom, float top, float near, float far) override
	{
		__m128 m[4];
		float freciW = 1.0f / (right - left);
		float freciH = 1.0f / (top - bottom);
		float frange = 1.0f / (far - near);

		__m128 rmem1 = _mm_set_ps(freciW, freciH, frange, 1.0f);
		__m128 rmem2 = _mm_set_ps((-left + right), -(top + bottom), -near, 1.0f);

		__m128 vals = rmem1;
		__m128 temp = _mm_setzero_ps();

		temp = _mm_move_ss(temp, vals);
		temp = _mm_add_ss(temp, temp);
		m[0] = temp;

		temp = vals;
		temp = _mm_and_ps(temp, { (float)0x00000000, (float)0xFFFFFFFF, (float)0x00000000, (float)0x00000000 });
		temp = _mm_add_ps(temp, temp);
		m[1] = temp;

		temp = vals;
		temp = _mm_and_ps(temp, { (float)0x00000000, (float)0xFFFFFFFF, (float)0x00000000, (float)0x00000000 });
		m[2] = temp;

		vals = _mm_mul_ps(vals, rmem2);
		m[3] = vals;

		return m;
	}
	__m128* mt_ortho(float width, float height, float near, float far) override
	{
		__m128 m[4];
		float frange = 1.0f / (far - near);

		__m128 rmem = {
			2.0f / width,
			2.0f / height,
			frange,
			-frange * near
		};

		__m128 vals = rmem;
		__m128 temp = _mm_setzero_ps();

		temp = _mm_move_ss(temp, vals);

		m[0] = temp;

		temp = vals;
		temp = _mm_and_ps(temp, { (float)0x00000000, (float)0xFFFFFFFF, (float)0x00000000, (float)0x00000000 });
		m[1] = temp;

		temp = _mm_setzero_ps();
		vals = _mm_shuffle_ps(vals, { 0.0f, 0.0f, 0.0f, 1.0f }, shuffle(3, 2, 3, 2));

		temp = _mm_shuffle_ps(temp, vals, shuffle(2, 0, 0, 0));
		m[2] = temp;

		temp = _mm_shuffle_ps(temp, vals, shuffle(3, 1, 0, 0));
		m[3] = temp;

		return m;
	}
	__m128* mt_lookat(__m128 eye, __m128 at, __m128 up) override
	{
		__m128 eyedir = _mm_sub_ps(at, eye);
		return mt_lookto(eye, eyedir, up);
	}
	__m128* mt_lookto(__m128 eye, __m128 dir, __m128 up) override
	{
		__m128 r2 = v3_norm(dir);
		__m128 r0 = v3_norm(v3_cross(up, r2));
		__m128 r1 = v3_cross(r2, r0);

		__m128 neye = _mm_mul_ps(eye, s_negone);

		__m128 d0 = v3_dot(r0, neye);
		__m128 d1 = v3_dot(r1,neye);
		__m128 d2 = v3_dot(r2, neye);

		__m128 m[4];
		m[0] = v_select(d0, r0, { (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 }).value();
		m[1] = v_select(d1, r1, { (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 }).value();
		m[2] = v_select(d2, r2, { (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 }).value();
		m[3] = { 0.0f, 0.0f, 0.0f, 1.0f };

		return mt_transpose(m);
	}
	__m128* mt_transform(__m128 pos, __m128 rot, __m128 scale) override
	{
		__m128 VScalingOrigin = v_select(__m128{ (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 }, _mm_setzero_ps(), __m128{ (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 });
		__m128 NegScalingOrigin = _mm_setzero_ps();

		__m128* MScalingOriginI = mt_translation(NegScalingOrigin);
		__m128* MScalingOrientation = mt_rotation(_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f));
		__m128* MScalingOrientationT = mt_transpose(MScalingOrientation);
		__m128* MScaling = mt_scaling(scale);
		__m128 VRotationOrigin = v_select((__m128{ (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 }), _mm_setzero_ps(), __m128{ (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 });
		__m128* MRotation = mt_rotation(rot);
		__m128 VTranslation = v_select((__m128{ (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 }), pos, __m128{ (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 });

		__m128* M;
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
	__m128* mt_translation(__m128 pos) override
	{

	}
	__m128* mt_scaling(__m128 scale) override
	{

	}
	__m128* mt_rotation(__m128 rot) override
	{

	}
};

static simd_math* s_instance;

void math::init()
{
	//https://gist.github.com/hi2p-perim/7855506
	int cpuinfo[4];
	__cpuid(cpuinfo, 1);

	if (!(cpuinfo[3] & (1 << 26) || false))
	{
		warn("cpu does not support the SSE2 instruction set! this could degrade performance.");
	}

	s_instance = new simd_sse2();
}

void math::kill()
{
	delete s_instance;
	s_instance = nullptr;
}
