#pragma once

#include <memory>

#include "alasthope/engine/aloo/drawable.h"

namespace engine
{
	namespace aloo
	{
		class font;
	}

	namespace drawables
	{
		class fps_counter : public engine::aloo::drawable
		{
		};

		std::shared_ptr<fps_counter> create_fps_counter(std::shared_ptr<aloo::font> const& font);
	}
}