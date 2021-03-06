#include "pch.h"
#pragma hdrstop

#include <unordered_map>

#include "structure_info.h"
#include "structure_infos.h"

namespace
{
	using namespace engine;

	std::string _none_name{ "none" };
	std::string _barracks_name{ "barracks" };
	std::string _solarpanel_name{ "solar panels" };
	std::string _algaefarm_name{ "algae farm" };
	std::string _watercollector_name{ "atmospheric water collector" };
	std::string _launchpad_name{ "launchpad" };

	structure_info _none{ 0, _none_name, 0, 0, 0, 0 };
	structure_info _barracks { 1, _barracks_name, -1, -2, -1, 5 };
	structure_info _solarpanel{ 2, _solarpanel_name, 2, 0, 0, -1 };
	structure_info _algaefarm{ 3, _algaefarm_name, -1, -2, 2, -1 };
	structure_info _watercollector{ 4, _watercollector_name, 0, 1, 0, -1 };
	structure_info _launchpad{ 5, _launchpad_name, -10, -1, -1, -1 };

	using cache_type = std::unordered_map<size_t, std::reference_wrapper<structure_info>>;

	template <typename mapT, typename itemT>
	void emplace(mapT& map, itemT& item)
	{
		map.emplace(item.id, item);
	}

	cache_type initialize_cache()
	{
		cache_type cache{};

		emplace(cache, _barracks);
		emplace(cache, _solarpanel);
		emplace(cache, _algaefarm);
		emplace(cache, _watercollector);
		emplace(cache, _launchpad);

		return std::move(cache);
	}

	structure_info const& lookup(size_t const id)
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
	structure_info const& lookup_structure_none()
	{
		return _none;
	}

	structure_info const& lookup_structure_info(size_t const id)
	{
		return lookup(id);
	}
}