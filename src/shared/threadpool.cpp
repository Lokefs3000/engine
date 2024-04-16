#include "../../src/pch.h"
#include "threadpool.h"

#include <vector>
#include <thread>
#include <mutex>
#include <future>
#include <chrono>
#include <queue>

#include <spdlog/spdlog.h>
#define info(...) spdlog::get("SHARED")->info(__VA_ARGS__)
#define warn(...) spdlog::get("SHARED")->warn(__VA_ARGS__)

#define max_task_pool 128

#define tt_state_idle 1						//the task is not in use
#define tt_state_request 2					//the task is not available for others (legacy)
#define tt_state_execute 4					//the task is executing
#define tt_state_await 8					//the task is awaiting execution
#define tt_state_finished tt_state_idle     //the task has finished

struct threadtask
{
	std::function<void()> func;
	std::atomic_uchar state{ tt_state_idle };
	std::atomic_bool requested{ false };
};

struct threadpool::local
{
	std::vector<std::jthread> threads;

	std::mutex awaitjob;
	std::condition_variable awaitjobvar;

	std::atomic_bool active{ true };

	std::queue<threadtask*> awaiting;
	std::list<threadtask*> taskpool;
};

static threadpool* s_instance;

void threadpool::threadproc()
{
	while (m_local->active)
	{
		if (!m_local->active)
			break;

		std::unique_lock lock(m_local->awaitjob);
		m_local->awaitjobvar.wait(lock, [this]() { return !m_local->active || !m_local->awaiting.empty(); });
		
		if (!m_local->awaiting.empty())
		{
			threadtask* active = m_local->awaiting.front();
			m_local->awaiting.pop();
			lock.unlock();

			active->func();

			active->func = nullptr;
			active->state = tt_state_finished;
		}
	}
}

void threadpool::init(uint32_t max)
{
	s_instance = new threadpool();
	s_instance->m_local = new local();
	
	uint32_t thcount = std::thread::hardware_concurrency();
	if (max > thcount)
	{
		warn("thread count is larger then found threads! target: {}, actual: {}", max, thcount);
		max = thcount;
	}
	info("using #", max, "/", thcount, " threads for pool!");

	for (size_t i = 0; i < max; i++)
	{
		s_instance->m_local->threads.push_back(std::move(std::jthread(&threadpool::threadproc, s_instance)));
		s_instance->m_local->threads[i].detach();
	}

	info("creating: #{} tasks for task pool.", max_task_pool);
	for (size_t i = 0; i < max_task_pool; i++)
	{
		s_instance->m_local->taskpool.push_back(new threadtask());
	}
}

void threadpool::kill()
{
	for (threadtask*& task : s_instance->m_local->taskpool)
	{
		if (task->state == tt_state_execute || task->requested)
		{
			info("found still executing task! awaiting..");
			s_instance->await(task);
		}

		delete task;
	}

	s_instance->m_local->active = false;
	s_instance->m_local->awaitjobvar.notify_all();
	s_instance->m_local->taskpool.clear();

	delete s_instance->m_local;
	s_instance->m_local = nullptr;

	delete s_instance;
	s_instance = nullptr;
}

threadtask* threadpool::start(std::function<void()> func)
{
	if (m_local->taskpool.empty())
	{
		info("cannot get threadtask because pool is empty!");
		return nullptr;
	}

	std::unique_lock lock(m_local->awaitjob);

	threadtask* task = nullptr;
	for (threadtask*& enu_task : m_local->taskpool)
		if (enu_task->state == tt_state_idle && !enu_task->requested)
			task = enu_task;

	if (task == nullptr)
	{
		for (threadtask*& enu_task : m_local->taskpool)
			if (!enu_task->requested)
				task = enu_task;

		if (task == nullptr)
		{
			info("failed to get available task!");
			return nullptr;
		}

		task->requested = true; //perhaps it should unlock the mutex while waiting to allow other threads access?
		while (task->state != tt_state_idle);
		task->requested = false;
	}

	task->state = tt_state_await;
	task->func = func;

	m_local->awaiting.push(task);

	lock.unlock();
	m_local->awaitjobvar.notify_one();

	return task;
}

void threadpool::await(threadtask* task)
{
	if (task == nullptr || task->state == tt_state_idle)
		return;

	while (task->state != tt_state_finished);
}

threadpool* threadpool::instance()
{
	return s_instance;
}
