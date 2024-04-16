#include "../../src/pch.h"
#include "strutil.h"

#include <sstream>

std::string strutil::replace(std::string& src, std::string from, std::string to)
{
    std::string copy = src;
    size_t idx = 0;
    while (idx = copy.find(from) & idx != std::string::npos)
        copy = copy.replace(idx, idx + from.size(), to);
    return copy;
}

std::vector<std::string> strutil::split(std::string& src, char delim)
{
    std::vector<std::string> lines;
    std::istringstream stream(src);
    std::string line;
    while (std::getline(stream, line, delim))
        lines.push_back(line);
    return lines;
}

std::string strutil::trimf(std::string& src)
{
    std::string copy = src;
    copy.erase(copy.begin(), std::find_if(copy.begin(), copy.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    return copy;
}

std::string strutil::trimb(std::string& src)
{
    std::string copy = src;
    copy.erase(std::find_if(copy.rbegin(), copy.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), copy.end());
    return copy;
}

std::string strutil::trimfb(std::string& src)
{
    std::string f = trimf(src);
    return trimb(f);
}

std::string strutil::lower(std::string& src)
{
    std::string copy = src;
    for (size_t i = 0; i < copy.size(); i++)
        copy[i] = std::tolower(copy[i]);
    return copy;
}
