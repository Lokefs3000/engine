#pragma once

#include <cstdint>
#include <random>

class randomgen
{
private:
	std::mt19937_64 m_rng;
public:
	randomgen(uint32_t seed);

	int32_t next_s32(int32_t min = -INT32_MAX, int32_t max = INT32_MAX);
	uint32_t next_u32(uint32_t min = 0, uint32_t max = UINT32_MAX);

	static randomgen shared();
};