#pragma once

#include <memory>

namespace engine
{
	namespace aloo
	{
		class font_manager;

		class environment
		{
		protected:
			virtual ~environment() {};
		public:
			std::shared_ptr<font_manager> const& get_font_manager() const
			{
				return get_font_manager_internal();
			}
		private:
			virtual std::shared_ptr<font_manager> const& get_font_manager_internal() const = 0;
		};

		std::shared_ptr<environment> create_environment();
	}
}