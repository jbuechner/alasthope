#pragma once

#include <array>
#include <chrono>

#include "utils.h"

namespace engine
{
	namespace aloo
	{
		class fps_component
		{
		public:
			using clock_type = std::chrono::high_resolution_clock;
			using value_type = std::chrono::nanoseconds;
			using measure_result = std::tuple<bool, value_type>;

			fps_component()
				: _previous_frame{ clock_type::now() }
			{
			}

			inline measure_result measure()
			{
				using namespace std::chrono_literals;

				auto const now = clock_type::now();
				auto const delta = now - _previous_frame;
				_previous_frame = now;

				auto const isSignificant{ is_significant(delta) };
				if (isSignificant)
				{
					auto const fps = std::chrono::duration_cast<std::chrono::nanoseconds>(1s) / delta;
					to_string(fps, &fpsBuffer[0]);
				}

				return std::move(std::make_tuple(isSignificant, delta));
			}

			inline static constexpr bool is_significant(value_type const& value)
			{
				using namespace std::chrono_literals;
				return value > 1000000ns;
			}

			inline char const* const text() const
			{
				return &fpsBuffer[0];
			}
		private:
			std::array<char, std::numeric_limits<uint64_t>::digits10> fpsBuffer{};
			std::chrono::time_point<clock_type, value_type> _previous_frame{};
		};

	}
}