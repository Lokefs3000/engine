#pragma once

#include <map>
#include <string>
#include <vector>

struct asset_reg
{
	virtual ~asset_reg() = default;
	virtual int type() = 0;
};

class resources
{
private:
	std::map<int32_t, asset_reg*> m_regs;

	std::vector<char> load_reg(asset_reg* reg);
public:
	static void init();
	static void kill();

	void load(std::string location);
	void load_fldr(std::string location);

	std::string get_string(int32_t id);

	bool exists(int32_t id);

	static resources* instance();
};