#pragma once

#include <string>
#include <vector>
#include <map>

enum class cfg_node_btype : uint8_t;

struct cfg_node
{
private:
	int m_int = 0;
	float m_float = 0.0f;
	bool m_bool = false;
	std::string m_str = "";

	std::map<std::string, cfg_node> m_nodes;
	std::vector<cfg_node> m_array;

	cfg_node* m_parent = nullptr;

	cfg_node_btype m_type;

	friend class cfgfileloader;
public:
	int as_int();
	float as_float();
	bool as_bool();
	std::string as_str();
	cfg_node& at(size_t i);
	cfg_node& at(std::string i);

	cfg_node& add(std::string name, int v);
	cfg_node& add(std::string name, float v);
	cfg_node& add(std::string name, bool v);
	cfg_node& add(std::string name, std::string v);
	cfg_node& add(std::string name, cfg_node v);

	std::map<std::string, cfg_node>& nodes();
	cfg_node_btype type();
};

class cfgfileloader
{
private:
	static void configure_new_node(cfg_node*& ref, std::string& line, std::string& tval, std::string& tname, size_t& i, cfg_node_btype& btype);
public:
	static cfg_node load(std::string& string);
	static std::string save(cfg_node& root);
};