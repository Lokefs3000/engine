#pragma once

class renderstate;

class renderer
{
private:
	struct local;

	local* m_local;
	renderstate* m_state;

	friend class engine;
public:
	static void init();
	static void kill();

	void execute();

	static renderer* instance();
};