#pragma once

struct window;

class engine
{
private:
	window* game_window = nullptr;
public:
	static void init(int argc, char* argv[]);
	static void kill();
	static void run();
};