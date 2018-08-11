#pragma once

#include <memory>
#include <glm.hpp>

namespace engine
{
	namespace aloo
	{
		class render_texture;
	}

	namespace drawables
	{
		struct grid_options
		{
			std::shared_ptr<aloo::render_texture const> spritesheet{ nullptr };
			glm::uvec2 tile_size{ 32, 32 };
		};
	}
}