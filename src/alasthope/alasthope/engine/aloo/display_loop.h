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

			inline std::list<std::shared_ptr<drawable>> const& children() const
			{
				return children_internal();
			}

			inline void append(std::shared_ptr<drawable> const& drawable)
			{
				append_internal(drawable);
			}
		private:
			virtual std::list<std::shared_ptr<drawable>> const& children_internal() const = 0;
			virtual void append_internal(std::shared_ptr<drawable> const& drawable) = 0;
		};
	}
}