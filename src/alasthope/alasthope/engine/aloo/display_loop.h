#pragma once

#include <memory>

#include "display.h"

namespace engine
{
	namespace aloo
	{
		class drawable;

		class display_loop
		{
		protected:
			virtual ~display_loop() {};
		public:
			virtual bool operator()() = 0;

			void set_root_drawable(std::shared_ptr<drawable> const& drawable)
			{
				set_root_drawable_internal(drawable);
			}
		private:
			virtual void set_root_drawable_internal(std::shared_ptr<drawable> const& drawable) = 0;
		};
	}
}