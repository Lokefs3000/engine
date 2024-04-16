#include "../../src/pch.h"
#include "randomgen.h"

static randomgen s_shared(130424);

randomgen::randomgen(uint32_t seed)
{
	m_rng.seed(seed);
}

int32_t randomgen::next_s32(int32_t min, int32_t max)
{
	std::uniform_int_distribution<std::mt19937_64::result_type> udist(min + INT32_MAX, max + INT32_MAX);
	return udist(m_rng) - INT32_MAX;
}

uint32_t randomgen::next_u32(uint32_t min, uint32_t max)
{
	std::uniform_int_distribution<std::mt19937_64::result_type> udist(min, max);
	return udist(m_rng);
}

randomgen randomgen::shared()
{
	return s_shared;
}
