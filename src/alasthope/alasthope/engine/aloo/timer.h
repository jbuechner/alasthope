#pragma once

#include <memory>
#include <chrono>

namespace engine
{
	namespace aloo
	{
		class timer
		{
		public:
			virtual ~timer() {};

			inline void wait_for_tick() const
			{
				wait_for_tick_internal();
			}
		private:
			virtual void wait_for_tick_internal() const = 0;
		};

		std::shared_ptr<timer> create_timer(std::chrono::milliseconds const& fpsStepTime);
	}
}