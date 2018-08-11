#include "pch.h"
#pragma hdrstop

#include "path_utils.h"

std::filesystem::path make_local_path(std::filesystem::path const& path)
{
	auto copy = path;
	auto appdir = std::filesystem::absolute(os::get_process_module_path());
	if (copy.is_relative())
	{
		copy = std::filesystem::absolute(appdir / copy);
	}

	auto copyIter = copy.begin();
	for (auto& part : appdir)
	{
		if (copyIter == copy.end())
		{
			return {};
		}

		if (part != *copyIter)
		{
			return {};
		}

		copyIter++;
	}

	return copy;
}