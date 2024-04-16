#pragma once

#include <string>
#include <vector>

class strutil
{
public:
	static std::string replace(std::string& src, std::string from, std::string to);
	static std::vector<std::string> split(std::string& src, char delim);

	static std::string trimf(std::string& src);
	static std::string trimb(std::string& src);
	static std::string trimfb(std::string& src);

	static std::string lower(std::string& src);
};