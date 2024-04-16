#pragma once

#include "rhibase.h"

class dx11 : public rhibase
{
private:
	struct local;
	local* m_local = nullptr;

	friend class graphics;
public:
	void init(window* win) override;
	void kill() override;

	void present() override;

	command_queue* create_cmd_queue() override;
	void clear_cmd_queue(command_queue* queue) override;
	void execute_cmd_queues(command_queue* queue[], int count) override;

	window_buffers* create() override;
	void clear_win_buffers(command_queue* queue, window_buffers* bufs, float rgba[4]) override;
};