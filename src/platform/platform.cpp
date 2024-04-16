#include "../../src/pch.h"
#include "platform.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

static std::shared_ptr<spdlog::logger> _logger_stdout = nullptr;
#define info(...) _logger_stdout->info(__VA_ARGS__)

#include "plwindow.h"

void platform::init()
{
	_logger_stdout = spdlog::stdout_color_st("PLATFORM");

	info("initialize platform");

	plwindow::init();
}

void platform::kill()
{
	info("kill platform");

	plwindow::kill();

	_logger_stdout.reset();
}
