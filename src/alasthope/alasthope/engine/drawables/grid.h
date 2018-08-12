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

			inline std::optional<glm::uvec2 const> get_coordinate_at_mouse_position(glm::uvec2 const& position) const
			{
				return get_coordinate_at_mouse_position_internal(position);
			}

			inline void set_cursor_visible(bool const& value) const
			{
				set_cursor_visible_internal(value);
			}

			inline void set_cursor_position(glm::uvec2 const& tile_coordinate) const
			{
				set_cursor_position_internal(tile_coordinate);
			}

			inline void set_grid(std::shared_ptr<rectangular_grid const> const& grid)
			{
				set_grid_internal(grid);
			}

		private:
			virtual std::optional<glm::uvec2 const> get_coordinate_at_mouse_position_internal(glm::uvec2 const& position) const = 0;
			virtual void set_cursor_visible_internal(bool const& value) const = 0;
			virtual void set_cursor_position_internal(glm::uvec2 const& tile_coordinate) const = 0;
			virtual void set_grid_internal(std::shared_ptr<rectangular_grid const> const& grid) = 0;
		};

		std::shared_ptr<grid> create_grid(grid_options const& options);
	}
}