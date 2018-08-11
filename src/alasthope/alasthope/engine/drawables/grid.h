#pragma once

#include <optional>
#include <memory>

#include "alasthope/engine/aloo/drawable.h"
#include "grid_options.h"

namespace engine
{
	class rectangular_grid;

	namespace drawables
	{
		class grid : public engine::aloo::drawable
		{
		public:
			virtual ~grid() {};

			std::optional<glm::uvec2 const> get_coordinate_at_mouse_position(glm::uvec2 const& position) const
			{
				return get_coordinate_at_mouse_position_internal(position);
			}

			void set_cursor_visible(bool const& value) const
			{
				set_cursor_visible_internal(value);
			}

			void set_cursor_position(glm::uvec2 const& tile_coordinate) const
			{
				set_cursor_position_internal(tile_coordinate);
			}

		private:
			virtual std::optional<glm::uvec2 const> get_coordinate_at_mouse_position_internal(glm::uvec2 const& position) const = 0;
			virtual void set_cursor_visible_internal(bool const& value) const = 0;
			virtual void set_cursor_position_internal(glm::uvec2 const& tile_coordinate) const = 0;
		};

		std::shared_ptr<grid> create_grid(grid_options const& options, std::shared_ptr<engine::rectangular_grid const> const& grid);
	}
}