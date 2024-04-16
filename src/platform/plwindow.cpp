#include "plwindow.h"
#include "plwindow.h"
#include "plwindow.h"
#include "plwindow.h"
#include "../../src/pch.h"
#include "plwindow.h"

#include <spdlog/spdlog.h>
#define info(...) spdlog::get("PLATFORM")->info(__VA_ARGS__);
#define warn(...) spdlog::get("PLATFORM")->warn(__VA_ARGS__);
#define error(...) spdlog::get("PLATFORM")->error(__VA_ARGS__);
#define critical(...) spdlog::get("PLATFORM")->critical(__VA_ARGS__);

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#undef NOWINOFFSETS
#include <Windows.h>

const char* win32_wclass_name = "engine_wclass";

#define call(func, ...) win32_##func(__VA_ARGS__)
#else
#define call(func, ...) critical("no corresponding plwindow call for function '{}'", #func)
#endif

static plwindow* s_instance = nullptr;

struct window {};

#ifdef _WIN32
LRESULT win32_window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct win32_window : window
{
	HWND hwnd = nullptr;
	bool isClosed = false;
};

void plwindow::win32_init()
{
	WNDCLASSEXA wndclass{};
	wndclass.cbSize = sizeof(wndclass);
	wndclass.lpfnWndProc = win32_window_proc;
	wndclass.lpszClassName = win32_wclass_name;
	wndclass.hInstance = GetModuleHandleW(nullptr);
	ATOM aret = RegisterClassExA(&wndclass);
	if (aret == 0)
		error("failed to register window class: {}", GetLastError());
}

void plwindow::win32_kill()
{
	bool bret = UnregisterClassA(win32_wclass_name, GetModuleHandleA(nullptr));
	if (!bret)
		error("failed to unregister class: {}", GetLastError());
}

window* plwindow::win32_create_window(char* title, int width, int height)
{
	win32_window* window = new win32_window();

	window->hwnd = CreateWindowExA(0,
		win32_wclass_name,
		title,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		nullptr,
		nullptr,
		GetModuleHandleA(nullptr),
		nullptr);
	if (window->hwnd == nullptr)
		error("failed to create window: {}", GetLastError());

	long ret = SetWindowLongPtrA(window->hwnd, -21, reinterpret_cast<LONG_PTR>((void*)window));
	if (ret < 0)
		error("failed to set window userdata: {}", GetLastError());

	return window;
}

void plwindow::win32_destroy_window(window* win)
{
	win32_window* win32 = (win32_window*)win;
	if (win32->hwnd == nullptr)
		return;
	DestroyWindow(win32->hwnd);
	win32->hwnd = nullptr;
}

bool plwindow::win32_window_closed(window* win)
{
	return static_cast<win32_window*>(win)->isClosed;
}

void plwindow::win32_pump_events()
{
	MSG msg{};
	while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		bool ret = TranslateMessage(&msg);
		if (!ret)
			DispatchMessageA(&msg);
		else
			warn("failed to translate message! an event might not have been passed..");
	}
}

void* plwindow::win32_get_hwnd(window* win)
{
	return static_cast<win32_window*>(win)->hwnd;
}

LRESULT win32_window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	win32_window* windata = reinterpret_cast<win32_window*>((LONG_PTR)GetWindowLongPtrA(hWnd, -21));
	if (windata == nullptr)
		warn("windata is null! some events likely wont work.");

	switch (uMsg)
	{
	case WM_CLOSE:
	{
		if (windata != nullptr)
			windata->isClosed = true;
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProcA(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
#endif

void plwindow::init()
{
	s_instance = new plwindow();

	s_instance->call(init);
}

void plwindow::kill()
{
	s_instance->call(kill);

	delete s_instance;
	s_instance = nullptr;
}

window* plwindow::create_window(char* title, int width, int height)
{
	return call(create_window, title, width, height);
}

void plwindow::destroy_window(window* win)
{
	call(destroy_window, win);
}

bool plwindow::window_closed(window* win)
{
	return call(window_closed, win);
}

void plwindow::pump_events()
{
	call(pump_events);
}

plwindow* plwindow::instance()
{
	return s_instance;
}
