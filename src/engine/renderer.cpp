#include "renderer.h"
#include "../../src/pch.h"

#include "graphics.h"
#include "rhibase.h"

static renderer* s_instance = nullptr;

struct renderer::local
{
	rhibase* rhi;
	window_buffers* window_bufs;
	command_queue* primary_cmd;
};

void renderer::init()
{
	s_instance = new renderer();

	s_instance->m_local = new local();
	s_instance->m_local->rhi = graphics::instance();
	s_instance->m_local->window_bufs = s_instance->m_local->rhi->create();
	s_instance->m_local->primary_cmd = s_instance->m_local->rhi->create_cmd_queue();
}

void renderer::kill()
{
	delete s_instance->m_local->primary_cmd;
	delete s_instance->m_local->window_bufs;
	delete s_instance->m_local;

	delete s_instance;
	s_instance = nullptr;
}

void renderer::execute()
{
	m_local->rhi->clear_cmd_queue(m_local->primary_cmd);

	float clearCol[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_local->rhi->clear_win_buffers(m_local->primary_cmd, m_local->window_bufs, clearCol);

	command_queue* queues[] = { m_local->primary_cmd };
	m_local->rhi->execute_cmd_queues(queues, 1);
	m_local->rhi->present();
}

renderer* renderer::instance()
{
	return s_instance;
}
