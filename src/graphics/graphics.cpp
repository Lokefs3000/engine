#include "../../src/pch.h"
#include "graphics.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

static std::shared_ptr<spdlog::logger> _logger_stdout = nullptr;
#define info(...) _logger_stdout->info(__VA_ARGS__)

#include "rhibase.h"
#include "dx11.h"

static rhibase* s_instance = nullptr;

void graphics::init()
{
	_logger_stdout = spdlog::stdout_color_st("GRAPHICS");

	info("initializing graphics");

	s_instance = new dx11();
}

void graphics::kill()
{
	info("kill graphics");

	delete s_instance;
	s_instance = nullptr;
}

rhibase* graphics::instance()
{
	return s_instance;
}
