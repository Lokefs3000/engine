#include "../../src/pch.h"
#include "fileutil.h"

#include <spdlog/spdlog.h>
#include <fstream>
#include <filesystem>

#define error(...) spdlog::get("SHARED")->error(__VA_ARGS__)

struct filestream_out : public filestream
{
    std::ofstream stream;
};

std::vector<char> fileutil::readbin(std::string location)
{
    std::ifstream stream;

    stream.open<std::string>(location, std::ios::binary | std::ios::ate);
    if (!stream.is_open())
    {
        error("failed to open file: {}", location);
        return {};
    }

    std::vector<char> buf{};
    buf.resize(stream.tellg());

    stream.seekg(0);
    stream.read(buf.data(), sizeof(char) * buf.size());
    stream.close();

    return buf;
}

std::string fileutil::readtxt(std::string location)
{
    std::ifstream stream;

    stream.open<std::string>(location, std::ios::ate);
    if (!stream.is_open())
    {
        error("failed to open file: {}", location);
        return {};
    }

    std::string buf{};
    buf.resize(stream.tellg(), ' ');

    stream.seekg(0);
    stream.read(buf.data(), sizeof(char) * buf.size());
    stream.close();

    return buf;
}

void fileutil::writebin(std::string location, std::vector<char> contents)
{
    std::ofstream stream;

    stream.open<std::string>(location, std::ios::binary);
    if (!stream.is_open())
    {
        error("failed to open file: {}", location);
        return;
    }

    if (!contents.empty())
        stream.write(contents.data(), sizeof(char) * contents.size());

    stream.flush();
    stream.close();
}

void fileutil::writetxt(std::string location, std::string& contents)
{
    std::ofstream stream;

    stream.open<std::string>(location);
    if (!stream.is_open())
    {
        error("failed to open file: {}", location);
        return;
    }

    if (!contents.empty())
        stream.write(contents.data(), sizeof(contents[0]) * contents.size());

    stream.flush();
    stream.close();
}

filestream* fileutil::openbin(std::string location)
{
    filestream_out* stream = new filestream_out();
    
    stream->stream.open(location, std::ios::binary);
    if (!stream->stream.is_open())
    {
        error("failed to open stream at: {}", location);
        delete stream;
        return nullptr;
    }

    return stream;
}

filestream* fileutil::opentxt(std::string location)
{
    filestream_out* stream = new filestream_out();

    stream->stream.open(location);
    if (!stream->stream.is_open())
    {
        error("failed to open stream at: {}", location);
        delete stream;
        return nullptr;
    }

    return stream;
}

void fileutil::close(filestream* stream)
{
    if (stream != nullptr)
    {
        static_cast<filestream_out*>(stream)->stream.close();
        delete static_cast<filestream_out*>(stream);
    }
}

void fileutil::write(filestream* stream, std::vector<char> contents)
{
    if (stream != nullptr)
        static_cast<filestream_out*>(stream)->stream.write(contents.data(), sizeof(contents[0]) * contents.size());
}

void fileutil::write(filestream* stream, std::string& contents)
{
    if (stream != nullptr)
        static_cast<filestream_out*>(stream)->stream.write(contents.data(), sizeof(contents[0]) * contents.size());
}

void fileutil::write(filestream* stream, char* contents, size_t size)
{
    if (stream != nullptr)
        static_cast<filestream_out*>(stream)->stream.write(contents, size);
}

bool fileutil::exists(std::string location)
{
    return std::filesystem::exists(location);
}
