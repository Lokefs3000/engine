#include "../../src/pch.h"
#include "shared.h"

#include <spdlog/sinks/stdout_color_sinks.h>

static std::shared_ptr<spdlog::logger> _logger_stdout = nullptr;
#define info(...) _logger_stdout->info(__VA_ARGS__)

void shared::init()
{
	_logger_stdout = spdlog::stdout_color_mt("SHARED");

	info("initialize shared");
}

void shared::kill()
{
	info("kill shared");

	_logger_stdout.reset();
}
