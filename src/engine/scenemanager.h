#pragma once

#include <cstdint>
#include <vector>

class scene;

class scenemanager
{
private:
	std::vector<scene*> m_scenes;
public:
	static void init();
	static void kill();

	void append(int32_t id);

	static scenemanager* instance();
};