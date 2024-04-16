#pragma once

#include <vector>
#include <string>

struct filestream {};

class fileutil
{
public:
	static std::vector<char> readbin(std::string location);
	static std::string readtxt(std::string location);

	static void writebin(std::string location, std::vector<char> contents);
	static void writetxt(std::string location, std::string& contents);

	static filestream* openbin(std::string location);
	static filestream* opentxt(std::string location);
	static void close(filestream* stream);

	static void write(filestream* stream, std::vector<char> contents);
	static void write(filestream* stream, std::string& contents);
	static void write(filestream* stream, char* contents, size_t size);

	static bool exists(std::string location);
};