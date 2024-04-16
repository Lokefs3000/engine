#pragma once

struct window;

struct window_buffers
{
	virtual ~window_buffers() = default;
};
struct command_queue
{
	virtual ~command_queue() = default;
};

class rhibase
{
private:
	friend class graphics;
public:
	virtual void init(window* win) = 0;
	virtual void kill() = 0;

	virtual void present() = 0;

	virtual command_queue* create_cmd_queue() = 0;
	virtual void clear_cmd_queue(command_queue* queue) = 0;
	virtual void execute_cmd_queues(command_queue* queue[], int count) = 0;

	virtual window_buffers* create() = 0;
	virtual void clear_win_buffers(command_queue* queue, window_buffers* bufs, float rgba[4]) = 0;
};