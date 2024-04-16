#include "engine.h"

int main(int argc, char* argv[])
{
	engine::init(argc, argv);
	engine::run();
	engine::kill();

	return 0;
}