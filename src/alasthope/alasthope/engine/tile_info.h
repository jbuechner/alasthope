#pragma once

#include <memory>
#include <glm.hpp>

namespace engine
{
	class terrain_info;

	class tile_info
	{
	public:
		virtual ~tile_info() {};

		glm::uvec2 const& coordinate() const
		{
			return coordinate_internal();
		}

		terrain_info const& terrain() const
		{
			return terrain_internal();
		}

		size_t const& terrain_variation() const
		{
			return terrain_variation_internal();
		}

		void set_terrain(terrain_info const& terrain)
		{
			set_terrain_internal(terrain);
		}

		void set_terrain_variation(size_t const& value)
		{
			return set_terrain_variation_internal(value);
		}

	private:
		virtual glm::uvec2 const& coordinate_internal() const = 0;
		virtual terrain_info const& terrain_internal() const = 0;
		virtual void set_terrain_internal(terrain_info const& terrain) = 0;
		virtual size_t const& terrain_variation_internal() const = 0;
		virtual void set_terrain_variation_internal(size_t const& value) = 0;
	};

	std::shared_ptr<tile_info> create_tile_info(glm::uvec2 const& coordinate);
}