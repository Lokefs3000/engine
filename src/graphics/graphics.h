#pragma once

class rhibase;

class graphics
{
public:
	static void init();
	static void kill();

	static rhibase* instance();
};