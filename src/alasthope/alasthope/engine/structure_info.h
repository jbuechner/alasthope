#pragma once

#include <string>

namespace engine
{
	class structure_info
	{
	public:
		structure_info(size_t const& id_, std::string_view const& name_)
			: id{ id_ }, name{ name_ }
		{
		}

		size_t const id;
		std::string_view const name;
	};
}