#pragma once

#include <list>

namespace engine
{
	namespace aloo
	{
		class drawable;
	}

	namespace drawables
	{
		namespace details
		{
			using namespace engine::aloo;

			template <typename T>
			class drawables_parental
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
}