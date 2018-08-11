#pragma once

#include <memory>
#include <list>

namespace engine
{
	namespace aloo
	{
		class drawable;

		template <typename T>
		class parental
		{
		public:
			std::weak_ptr<drawable>& parent()
			{
				return _parent;
			}
		private:
			std::weak_ptr<drawable> _parent{};
		};
	}
}