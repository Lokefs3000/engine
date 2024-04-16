#include "../../src/pch.h"
#include "scene.h"

#include <spdlog/spdlog.h>
#define error(...) spdlog::get("ENGINE")->error(__VA_ARGS__)

#include "resources.h"
#include "cfgfleloader.h"

void scene::loadtxt(int32_t id)
{
	std::string src = resources::instance()->get_string(id);
	cfg_node root = cfgfileloader::load(src);
}

scene* scene::load(int32_t id)
{
	if (!resources::instance()->exists(id))
	{
		error("scene with id: {} does not exist!", id);
		return nullptr;
	}

	bool is_txt = true;

	scene* created = new scene();

	if (is_txt)
		created->loadtxt(id);
	else
		;

	return created;
}
