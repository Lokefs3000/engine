#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING

#include <spdlog/sinks/stdout_color_sinks.h>

#include "shared.h"

#include "util_indexfiles.h"
#include "util_compilemesh.h"

int main(int argc, char* argv[])
{
	if (argc < 2)
		return EXIT_SUCCESS;

	std::shared_ptr<spdlog::logger> _logger = spdlog::stdout_color_st("PROJECTC");
	
	std::vector<std::string> args;
	for (size_t i = 1; i < argc; i++)
		args.push_back(argv[i]);

	shared::init();

	std::string action = argv[1];
	if (action == "-idxfiles")
		util_indexfiles::execute(args);
	else if (action == "-cmesh")
		util_compilemesh::execute(args);

	shared::kill();

	return EXIT_SUCCESS;
}