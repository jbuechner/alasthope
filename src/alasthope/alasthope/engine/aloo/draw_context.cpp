#include "pch_allegro.h"
#pragma hdrstop

#include "display.h"
#include "draw_context.h"

namespace engine
{
	namespace aloo
	{
		draw_context::draw_context(std::shared_ptr<display> const& display)
			: _display{ display }
		{
		}

		fps_component::measure_result& draw_context::fps_measure_result()
		{
			return _fps_measure_result;
		}

		std::string_view& draw_context::fps_as_string()
		{
			return _fps_as_string;
		}

		std::shared_ptr<display> const& draw_context::get_display()
		{
			return _display;
		}
	}
}