#include "../../src/pch.h"
#include "scenemanager.h"

#include "scene.h"

#include <spdlog/spdlog.h>
#define error(...) spdlog::get("ENGINE")->error(__VA_ARGS__)

static scenemanager* s_instance;

void scenemanager::init()
{
	s_instance = new scenemanager();
}

void scenemanager::kill()
{
	for (scene*& sc : s_instance->m_scenes)
	{
		delete sc;
		sc = nullptr;
	}
	s_instance->m_scenes.clear();

	delete s_instance;
	s_instance = nullptr;
}

void scenemanager::append(int32_t id)
{
	scene* sc = scene::load(id);
	if (sc != nullptr)
		s_instance->m_scenes.push_back(sc);
	else
		error("failed to load scene with id: {}", id);
}

scenemanager* scenemanager::instance()
{
	return s_instance;
}
