#include "pch.h"

#include <unordered_map>

#include "terrain_infos.h"

namespace
{
	using namespace engine;

	std::string _none_name{ "n/a" };
	std::string _sand_name{ "sands" };
	std::string _badland_name { "badlands" };

	terrain_info _none { 0, _none_name  };
	terrain_info _sand { 1, _sand_name };
	terrain_info _badland { 2, _badland_name };

	using cache_type = std::unordered_map<size_t, std::reference_wrapper<terrain_info>>;

	cache_type initialize_cache()
	{
		cache_type cache{};

		cache.emplace(_sand.id, _sand);
		cache.emplace(_badland.id, _badland);

		return std::move(cache);
	}

	terrain_info const& lookup(size_t const id)
	{
		static cache_type cache = initialize_cache();

		auto const iter = cache.find(id);
		if (iter != cache.cend())
		{
			return iter->second;
		}

		return _none;
	}
}

namespace engine
{
	terrain_info const& lookup_terrain_none()
	{
		return _none;
	}

	terrain_info const& lookup_terrain_info(size_t const id)
	{
		return lookup(id);
	}
}