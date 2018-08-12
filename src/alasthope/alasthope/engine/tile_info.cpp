#include "pch.h"
#pragma hdrstop

#include <algorithm>
#include "terrain_info.h"
#include "terrain_infos.h"
#include "structure_info.h"
#include "structure_infos.h"
#include "tile_info.h"

namespace
{
	using namespace engine;

	class tile_info_internal : public tile_info
	{
	public:
		tile_info_internal(glm::uvec2 const& coordinate)
			: _coordinate{ coordinate }, _terrain_info{ lookup_terrain_none() }, _structure_info{ lookup_structure_none() }
		{
		}
	private:
		glm::uvec2 const& coordinate_internal() const override
		{
			return _coordinate;
		}

		terrain_info const& terrain_internal() const override
		{
			return _terrain_info;
		}

		void set_terrain_internal(terrain_info const& terrain) override
		{
			_terrain_info = terrain;
		}

		size_t const& terrain_variation_internal() const override
		{
			return _terrain_variation;
		}

		void set_terrain_variation_internal(size_t const& value) override
		{
			_terrain_variation = value;
		}

		structure_info const& structure_internal() const override
		{
			return _structure_info;
		}

		void set_structure_internal(structure_info const& structure) override
		{
			_structure_info = structure;
		}

		std::size_t waste_factor_internal() const override
		{
			return _waste_factor;
		}

		void change_waste_factor_internal(int32_t const& value) override
		{
			auto newValue = static_cast<int32_t>(_waste_factor) + value;
			_waste_factor = static_cast<size_t>(std::max(0, std::min(100, newValue)));
		}

		bool is_still_in_game_internal() const override
		{
			return _waste_factor < 70;
		}

		std::size_t _waste_factor{ 0 };
		glm::uvec2 const _coordinate;
		std::reference_wrapper<terrain_info const> _terrain_info;
		size_t _terrain_variation{ 0 };
		std::reference_wrapper<structure_info const> _structure_info;
	};
}

namespace engine
{
	std::shared_ptr<tile_info> create_tile_info(glm::uvec2 const& coordinate)
	{
		return std::make_shared<tile_info_internal>(coordinate);
	}
}