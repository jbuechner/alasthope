#pragma once

#include <memory>
#include <glm.hpp>

namespace engine
{
	class terrain_info;
	class structure_info;

	class tile_info
	{
	public:
		virtual ~tile_info() {};

		inline glm::uvec2 const& coordinate() const
		{
			return coordinate_internal();
		}

		inline terrain_info const& terrain() const
		{
			return terrain_internal();
		}

		inline size_t const& terrain_variation() const
		{
			return terrain_variation_internal();
		}

		inline structure_info const& structure() const
		{
			return structure_internal();
		}

		inline void set_structure(structure_info const& structure)
		{
			set_structure_internal(structure);
		}

		inline void set_terrain(terrain_info const& terrain)
		{
			set_terrain_internal(terrain);
		}

		inline void set_terrain_variation(size_t const& value)
		{
			return set_terrain_variation_internal(value);
		}

		inline size_t waste_factor() const
		{
			return waste_factor_internal();
		}

		inline void change_waste_factor(double const& value)
		{
			change_waste_factor_internal(value);
		}

		inline bool is_still_in_game() const
		{
			return is_still_in_game_internal();
		}

		inline bool is_active() const
		{
			return is_active_internal();
		}

	private:
		virtual glm::uvec2 const& coordinate_internal() const = 0;
		virtual terrain_info const& terrain_internal() const = 0;
		virtual void set_terrain_internal(terrain_info const& terrain) = 0;
		virtual size_t const& terrain_variation_internal() const = 0;
		virtual void set_terrain_variation_internal(size_t const& value) = 0;
		virtual structure_info const& structure_internal() const = 0;
		virtual void set_structure_internal(structure_info const& structure) = 0;
		virtual size_t waste_factor_internal() const = 0;
		virtual void change_waste_factor_internal(double const& value) = 0;
		virtual bool is_still_in_game_internal() const = 0;
		virtual bool is_active_internal() const = 0;
	};

	std::shared_ptr<tile_info> create_tile_info(glm::uvec2 const& coordinate);
}