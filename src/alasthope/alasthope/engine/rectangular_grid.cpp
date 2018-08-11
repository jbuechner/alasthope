#include "pch.h"
#pragma hdrstop

#include <random>
#include <vector>

#include "terrain_info.h"
#include "terrain_infos.h"

#include "tile_info.h"
#include "rectangular_grid.h"

namespace
{
	using namespace engine;

	class rectangular_grid_internal : public rectangular_grid
	{
	public:
		rectangular_grid_internal(glm::uvec2 const& size)
			: _size{ size }
		{
			_storage.resize(size.x * size.y);

			fill();
		}

	private:
		std::shared_ptr<tile_info> const& lookup_internal(glm::uvec2 const& coordinate) const override
		{
			return lookup_ref(coordinate);
		}

		glm::uvec2 const& size_internal() const override
		{
			return _size;
		}

		inline std::shared_ptr<tile_info>& lookup_ref(glm::uvec2 const& coordinate) const
		{
			return _storage[coordinate.x + coordinate.y * _size.x];
		}

		void fill()
		{
			std::mt19937_64 mt{};
			auto const f = (mt.max() - mt.min()) / 4;

			for (glm::uvec2::value_type x = 0; x < _size.x; x++)
			{
				for (glm::uvec2::value_type y = 0; y < _size.y; y++)
				{
					glm::uvec2 coordinate{ x, y };
					auto& tile = lookup_ref(coordinate);
					tile = create_tile_info(coordinate);

					tile->set_terrain(lookup_terrain_info(1));
					tile->set_terrain_variation(mt() / f);
				}
			}
		}

		mutable std::vector<std::shared_ptr<tile_info>> _storage{};
		glm::uvec2 const _size;
	};
}

namespace engine
{
	std::shared_ptr<rectangular_grid> create_rectangular_grid(glm::uvec2 const& size)
	{
		return std::make_shared<rectangular_grid_internal>(size);
	}
}