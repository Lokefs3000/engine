#include "../../src/pch.h"
#include "resources.h"

#include <spdlog/sinks/stdout_color_sinks.h>

static std::shared_ptr<spdlog::logger> _logger_stdout = nullptr;
#define info(...) _logger_stdout->info(__VA_ARGS__)
#define warn(...) _logger_stdout->warn(__VA_ARGS__)
#define error(...) _logger_stdout->error(__VA_ARGS__)

#include "fileutil.h"
#include "cfgfleloader.h"
#include "strutil.h"

#define package_id { 'P', 'A', 'K' }
#define package_ver 1

struct package_header
{
	char id[3] = package_id;
	uint16_t version = package_ver;

	uint32_t resourceCount = 0;
};

struct package_resource
{
	int32_t assetId;

	size_t srcoffset;
	size_t srclength;

	bool compressed;
	size_t srclength_c;
};

struct fldr_asset_reg : public asset_reg
{
	std::string path;

	~fldr_asset_reg() override
	{}

	int type() override
	{
		return 1;
	}
};

static resources* s_instance;

std::vector<char> resources::load_reg(asset_reg* reg)
{
	if (reg->type() == 1)
	{
		fldr_asset_reg* fldr = (fldr_asset_reg*)reg;
		return fileutil::readbin(fldr->path);
	}
	else
	{

	}

	return {}; //failsafe
}

void resources::init()
{
	_logger_stdout = spdlog::stdout_color_st("RESSYTEM");

	info("initializing resources");

	s_instance = new resources();
}

void resources::kill()
{
	info("kill resources");

	for (std::pair<const int32_t, asset_reg*> reg : s_instance->m_regs)
		delete reg.second;
	s_instance->m_regs.clear();

	delete s_instance;
	s_instance = nullptr;

	_logger_stdout.reset();
}

void resources::load(std::string location)
{
	if (!fileutil::exists(location))
	{
		error("failed to find package file at: \"{}\"", location);
		return;
	}

	package_header header{};
	std::vector<package_resource> resources{};

	std::vector<char> packagesrc = fileutil::readbin(location);
	if (packagesrc.empty())
	{
		warn("skipped loading of package: {}, beacuse it is either empty or an error happened while reading", location);
		return;
	}

	if (packagesrc.size() < sizeof(package_header))
	{
		error("package: {}, is not large enough for a header!", location);
		return;
	}
	memcpy(&header, packagesrc.data(), sizeof(package_header));

	if (packagesrc.size() < sizeof(package_header) + sizeof(package_resource) * header.resourceCount)
	{
		error("package: {}, is not large enough for a specified resource count!", location);
		return;
	}
	resources.resize(header.resourceCount);
	memcpy(resources.data(), packagesrc.data() + sizeof(package_header), sizeof(package_resource) * header.resourceCount);
}

void resources::load_fldr(std::string location)
{
	info("appending resources from project folder: {}", location);

	if (!fileutil::exists(location))
	{
		error("failed to find project folder at: \"{}\"", location);
		return;
	}

	std::string globalAssetReg = location + "/library/gar.acf";
	std::string garSrc = fileutil::readtxt(globalAssetReg);
	cfg_node gar = cfgfileloader::load(garSrc);

	int added = 0;
	for (std::pair<const std::string, cfg_node>& asset : gar.nodes())
	{
		int32_t id = std::stoi(asset.first);
		std::string path = asset.second.at("path").as_str();

		fldr_asset_reg* reg = new fldr_asset_reg();
		reg->path = location + "/" + path;
		m_regs.insert(std::make_pair(id, (asset_reg*)reg));
		
		added++;
	}

	info("appended #{} new entries into filesystem!", added);
}

std::string resources::get_string(int32_t id)
{
	if (!exists(id))
	{
		error("cannot find resource with id: {}", id);
		return "";
	}

	asset_reg* reg = m_regs.at(id);
	std::vector<char> src = load_reg(reg);

	std::string str_src = std::string(src.begin(), src.end());
	return strutil::trimfb(str_src);
}

bool resources::exists(int32_t id)
{
	return m_regs.contains(id);
}

resources* resources::instance()
{
	return s_instance;
}
