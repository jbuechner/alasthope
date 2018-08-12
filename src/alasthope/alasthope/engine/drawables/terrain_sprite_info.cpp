#include "pch.h"
#pragma hdrstop

#include <algorithm>
#include <vector>
#include <unordered_map>

#include "terrain_sprite_info.h"

namespace
{
	using namespace engine::drawables;

	using terrain_sprite_info_variation_list_type = std::vector<terrain_sprite_info>;

	class terrain_sprite_info_list
	{
	public:
		terrain_sprite_info_list(terrain_sprite_info_variation_list_type const& variations_)
			: variations{ std::move(variations_) }
		{
		}

		terrain_sprite_info_variation_list_type const variations;
	};

	terrain_sprite_info_list _none_sprite_info{ std::move<terrain_sprite_info_variation_list_type>({ { { 0, 0, 32, 32 } } }) };
	terrain_sprite_info_list _sand_sprite_info{ std::move<terrain_sprite_info_variation_list_type>({ { { 0, 32, 32, 32 } },{ { 0, 64, 32, 32 } },{ { 0, 96, 32, 32 } },{ { 0, 128, 32, 32 } } }) };
	terrain_sprite_info_list _badland_sprite_info{ std::move<terrain_sprite_info_variation_list_type>({ { { 0, 160, 32, 32 } },{ { 0, 192, 32, 32 } },{ { 0, 224, 32, 32 } },{ { 32, 224, 32, 32 } } }) };

	using terrain_sprite_info_cache_type = std::unordered_map<size_t, terrain_sprite_info_list>;

	terrain_sprite_info_cache_type initialize_terrain_sprite_info_cache()
	{
		terrain_sprite_info_cache_type cache{};

		cache.emplace(0, _none_sprite_info);
		cache.emplace(1, _sand_sprite_info);
		cache.emplace(2, _badland_sprite_info);

		return std::move(cache);
	}
}

namespace engine
{
	namespace drawables
	{
		terrain_sprite_info const& lookup_terrain_sprite_info(size_t const& id, size_t const& variation)
		{
			static terrain_sprite_info_cache_type cache{ std::move(initialize_terrain_sprite_info_cache()) };

			auto const iter = cache.find(id);
			if (iter != cache.cend())
			{
				auto& list = iter->second;
				auto const size = list.variations.size();
				return list.variations.at(std::max(static_cast<size_t>(0), std::min(std::max(static_cast<size_t>(0), size - 1), variation)));
			}
			return _none_sprite_info.variations.at(0);
		}

		terrain_sprite_info const& get_default_terrain_sprite_info()
		{
			return _none_sprite_info.variations.at(0);
		}
	}
}