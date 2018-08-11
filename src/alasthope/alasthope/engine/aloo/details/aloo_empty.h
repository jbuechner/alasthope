#pragma once

#include <memory>

namespace engine
{
	namespace aloo
	{
		namespace details
		{
			template<typename T>
			std::enable_if_t<std::is_pointer_v<T>, T> const& empty()
			{
				static T v{ nullptr };
				return v;
			}

			template <typename T>
			std::enable_if_t<!std::is_pointer_v<T>, T> const& empty()
			{
				static T v{};
				return v;
			}
		}
	}
}