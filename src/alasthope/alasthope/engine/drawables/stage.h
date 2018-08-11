#pragma once

#include <memory>

#include "alasthope/engine/aloo/drawable.h"

namespace engine
{
	namespace drawables
	{
		class stage : public engine::aloo::drawable
		{
		};

		std::shared_ptr<stage> create_stage();
	}
}