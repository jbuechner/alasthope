#pragma once

#include <glm.hpp>

namespace engine
{
	namespace drawables
	{
		class structure_sprite_info
		{
		public:
			structure_sprite_info()
				: source{}
			{
			}

			structure_sprite_info(glm::uvec2 const& source_)
				: source{ source_ }
			{
			}

			glm::uvec2 const source;
		};

		structure_sprite_info const& lookup_structure_sprite_info(size_t const& id);
		structure_sprite_info const& get_default_structure_sprite_info();
	}
}