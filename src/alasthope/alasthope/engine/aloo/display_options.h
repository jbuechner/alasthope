#pragma once

#include <glm.hpp>

namespace engine
{
	namespace aloo
	{
		struct display_options
		{
			glm::uvec2 display_size{};
			glm::uvec2 backbuffer_size{};
		};
	}
}