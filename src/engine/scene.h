#pragma once

#include <cstdint>

class scene
{
private:
	void loadtxt(int32_t id);
public:
	static scene* load(int32_t id);
};