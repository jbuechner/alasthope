#pragma once

#include <string>

namespace engine
{
	class structure_info
	{
	public:
		structure_info(size_t const& id_, std::string_view const& name_, int32_t const& electricityPerSecond, int32_t const& waterPerSecond, int32_t const& foodPerSecond, int32_t const& workforcePerSecond)
			: id{ id_ }, name{ name_ }, electricity_per_second{ electricityPerSecond }, water_per_second{ waterPerSecond }, food_per_second{ foodPerSecond }, workforce_per_second{ workforce_per_second }
		{
		}

		size_t const id;
		std::string_view const name;

		int32_t const electricity_per_second;
		int32_t const water_per_second;
		int32_t const food_per_second;
		int32_t const workforce_per_second;
	};
}