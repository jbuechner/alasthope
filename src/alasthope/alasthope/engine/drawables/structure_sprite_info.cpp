#include "pch.h"
#pragma hdrstop

#include <unordered_map>
#include "structure_sprite_info.h"

namespace
{
	using namespace engine::drawables;

	structure_sprite_info _none_sprite_info{ { 0, 0 } };
	structure_sprite_info _barracks_sprite_info{ { 64, 0 } };
	structure_sprite_info _solarpanel_sprite_info{ { 96, 0 } };
	structure_sprite_info _algaefarm_sprite_info{ { 128, 0 } };
	structure_sprite_info _watercollector_sprite_info{ { 160, 0 } };

	using structure_sprite_info_cache_type = std::unordered_map<size_t, structure_sprite_info&>;

	structure_sprite_info_cache_type initialize_cache()
	{
		structure_sprite_info_cache_type cache{};

		cache.emplace(0, _none_sprite_info);
		cache.emplace(1, _barracks_sprite_info);
		cache.emplace(2, _solarpanel_sprite_info);
		cache.emplace(3, _algaefarm_sprite_info);
		cache.emplace(4, _watercollector_sprite_info);

		return std::move(cache);
	}
}

namespace engine
{
	namespace drawables
	{
		structure_sprite_info const& lookup_structure_sprite_info(size_t const& id)
		{
			static auto cache{ std::move(initialize_cache()) };

			auto const iter = cache.find(id);
			if (iter != cache.cend())
			{
				return iter->second;
			}

			return _none_sprite_info;
		}

		structure_sprite_info const& get_default_structure_sprite_info()
		{
			return _none_sprite_info;
		}
	}
}