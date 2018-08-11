#pragma once

#include <memory>

#include "fps_component.h"

namespace engine
{
	namespace aloo
	{
		class display;

		class draw_context
		{
		public:
			draw_context(std::shared_ptr<display> const& display);

			fps_component::measure_result& fps_measure_result();
			std::string_view& fps_as_string();

			std::shared_ptr<display> const& get_display();
		private:
			std::shared_ptr<display> _display{ nullptr };
			fps_component::measure_result _fps_measure_result{};
			std::string_view _fps_as_string{};
		};
	}
}