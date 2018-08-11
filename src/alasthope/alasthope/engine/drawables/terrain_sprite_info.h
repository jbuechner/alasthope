#pragma once

#include <glm.hpp>

namespace engine
{
	namespace drawables
	{
		class terrain_sprite_info
		{
		public:
			terrain_sprite_info()
				: source{}
			{
			}

			terrain_sprite_info(glm::uvec2 const& source_)
				: source{ source_ }
			{
			}

			glm::uvec2 const source;
		};

		terrain_sprite_info const& lookup_terrain_sprite_info(size_t const& id, size_t const& variation);
		terrain_sprite_info const& get_default_terrain_sprite_info();
	}
}