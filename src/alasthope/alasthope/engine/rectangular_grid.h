#pragma once

#include <glm.hpp>
#include <memory>

namespace engine
{
	class tile_info;

	class rectangular_grid
	{
	public:
		virtual ~rectangular_grid() {};

		inline std::shared_ptr<tile_info> const& lookup(glm::uvec2 const& coordinate) const
		{
			return lookup_internal(coordinate);
		}

		inline glm::uvec2 const& size() const
		{
			return size_internal();
		}

		inline uint64_t seed() const
		{
			return seed_internal();
		}
	private:
		virtual std::shared_ptr<tile_info> const& lookup_internal(glm::uvec2 const& coordinate) const = 0;
		virtual glm::uvec2 const& size_internal() const = 0;
		virtual uint64_t seed_internal() const = 0;
	};

	std::shared_ptr<rectangular_grid> create_rectangular_grid(glm::uvec2 const& size, uint64_t const& seed);
}