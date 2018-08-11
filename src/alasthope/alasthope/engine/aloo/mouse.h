#pragma once

#include <memory>
#include <glm.hpp>

namespace engine
{
	namespace aloo
	{
		class mouse
		{
		public:
			virtual ~mouse() {};

			void process_events()
			{
				process_events_internal();
			}

			bool cursor_position_changed() const
			{
				return cursor_position_changed_internal();
			}

			glm::uvec2 position() const
			{
				return position_internal();
			}

		private:
			virtual void process_events_internal() = 0;
			virtual bool cursor_position_changed_internal() const = 0;
			virtual glm::uvec2 position_internal() const = 0;
		};

		std::shared_ptr<mouse> create_mouse(std::shared_ptr<environment const> const& environment);
	}
}