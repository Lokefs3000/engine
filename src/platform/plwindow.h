#pragma once

struct window;

class plwindow
{
private:
#ifdef _WIN32
	void win32_init();
	void win32_kill();

	window* win32_create_window(char* title, int width, int height);
	void win32_destroy_window(window* win);

	static bool win32_window_closed(window* win);
	static void win32_pump_events();
#endif
public:
	static void init();
	static void kill();

	window* create_window(char* title, int width, int height);
	void destroy_window(window* win);

#ifdef _WIN32
	static void* win32_get_hwnd(window* win);
#endif

	static bool window_closed(window* win);
	static void pump_events();

	static plwindow* instance();
};