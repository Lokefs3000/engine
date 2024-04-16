#include "util_indexfiles.h"

#include <filesystem>

#include "cfgfleloader.h"
#include "fileutil.h"
#include "randomgen.h"

void util_indexfiles::execute(std::vector<std::string> args)
{
	std::string projPath = "";

	for (size_t i = 0; i < args.size(); i++)
	{
		std::string& arg = args[i];
		if (arg == "--proj")
			projPath = args[++i];
	}

	randomgen rngnum(1424);

	std::string str_tbuf = fileutil::readtxt(projPath + "/library/gar.acf");
	cfg_node gar = cfgfileloader::load(str_tbuf);

	std::vector<std::string> already;
	std::vector<int> ids;

	for (std::pair<const std::string, cfg_node>& pair : gar.nodes())
	{
		already.push_back(pair.second.at("path").as_str());
		ids.push_back(std::stoi(pair.first));
	}

	//ugly loop change now the inside is horrible
	for (std::filesystem::path entry : std::filesystem::recursive_directory_iterator(projPath + "/content"))
	{
		if (std::filesystem::is_directory(entry))
			continue;
		std::string path = entry.string().substr(projPath.size() + 1);
		if (std::count(already.begin(), already.end(), path))
			continue;
		int32_t rng = rngnum.next_s32();
		while (std::count(ids.begin(), ids.end(), rng))
			rng = rngnum.next_s32();
		ids.push_back(rng);
		already.push_back(path);

		cfg_node& added = gar.add(std::to_string(rng), cfg_node {});
		added.add("path", path);
	}

	if (!std::count(already.begin(), already.end(), "library/project.acf"))
	{
		int32_t rng = rngnum.next_s32();
		while (std::count(ids.begin(), ids.end(), rng))
			rng = rngnum.next_s32();

		cfg_node& added = gar.add(std::to_string(rng), cfg_node{});
		added.add("path", std::string("library/project.acf"));
	}

	std::string serial = cfgfileloader::save(gar);
	fileutil::writetxt(projPath + "/library/gar.acf", serial);
}
