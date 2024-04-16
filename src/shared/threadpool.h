#pragma once

#include <atomic>
#include <functional>

struct threadtask;

class threadpool
{
private:
	struct local;
	local* m_local;

	void threadproc();
public:
	static void init(uint32_t max);
	static void kill();

	threadtask* start(std::function<void()> func);

	void await(threadtask* task);

	static threadpool* instance();
};