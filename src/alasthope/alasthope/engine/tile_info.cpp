#include "pch.h"
#pragma hdrstop

#include "terrain_info.h"
#include "terrain_infos.h"
#include "tile_info.h"

namespace
{
	using namespace engine;

	class tile_info_internal : public tile_info
	{
	public:
		tile_info_internal(glm::uvec2 const& coordinate)
			: _coordinate{ coordinate }, _terrain_info{ lookup_terrain_none() }
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

		glm::uvec2 const _coordinate;
		std::reference_wrapper<terrain_info const> _terrain_info;
		size_t _terrain_variation{ 0 };
	};
}

namespace engine
{
	std::shared_ptr<tile_info> create_tile_info(glm::uvec2 const& coordinate)
	{
		return std::make_shared<tile_info_internal>(coordinate);
	}
}