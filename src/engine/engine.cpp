#include "../../src/pch.h"
#include "engine.h"

#include <vector>
#include <string>

#include "shared.h"
#include "threadpool.h"
#include "cfgfleloader.h"

#include "resources.h"

#include "platform.h"
#include "plwindow.h"

#include "graphics.h"
#include "rhibase.h"

#include "renderer.h"
#include "renderstate.h"
#include "scenemanager.h"

static engine* s_instance = nullptr;

void engine::init(int argc, char* argv[])
{
	s_instance = new engine();

	std::vector<std::string> args;
	for (size_t i = 0; i < argc; i++)
		args.push_back(argv[i]);

	shared::init();
	threadpool::init(3/*4 - main*/);
	resources::init();
	platform::init();
	graphics::init();
	scenemanager::init();

	resources::instance()->load("game.pak");
	if (std::count(args.begin(), args.end(), "-pf"))
		resources::instance()->load_fldr(args[(std::find(args.begin(), args.end(), "-pf") - args.begin())+1]);

	s_instance->game_window = plwindow::instance()->create_window((char*)"game_window", 800, 600);
	graphics::instance()->init(s_instance->game_window);

	renderer::init();

	std::string proj_cfg_src = resources::instance()->get_string(0);
	cfg_node project_cfg = cfgfileloader::load(proj_cfg_src);

	scenemanager::instance()->append(project_cfg.at("Runtime").at("defscene").as_int());
}

void engine::kill()
{
	threadpool::kill();

	renderer::kill();
	graphics::instance()->kill();
	plwindow::instance()->destroy_window(s_instance->game_window);

	scenemanager::kill();
	graphics::kill();
	platform::kill();
	resources::kill();
	shared::kill();

	delete s_instance;
	s_instance = nullptr;
}

void engine::run()
{
	renderstate* active = new renderstate();
	renderer::instance()->m_state = new renderstate();

	while (!plwindow::window_closed(s_instance->game_window))
	{
		//give the newly refreshed state
		std::swap(active, renderer::instance()->m_state);

		//start the rendering task
		threadtask* task = threadpool::instance()->start(std::bind(&renderer::execute, renderer::instance()));

		//execute main logic
		plwindow::pump_events();

		active->soft_reset();

		//wait for rendering to end if its still going
		threadpool::instance()->await(task);
	}

	active->reset();
	delete active;
	renderer::instance()->m_state->reset();
	delete renderer::instance()->m_state;
}
