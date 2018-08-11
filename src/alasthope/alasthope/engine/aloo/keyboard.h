#pragma once

#include <memory>

namespace engine
{
	namespace aloo
	{
		class environment;

		enum class key_state
		{
			none,
			pressed
		};

		class keyboard
		{
		public:
			using key_event_listener = std::function<void(int const& key_code, key_state const& state)>;

			inline void add_listener(key_event_listener const& listener)
			{
				add_listener_internal(listener);
			}

			inline void process_events()
			{
				process_events_internal();
			}

			inline bool is_key_down(int const& key_code) const
			{
				return is_key_down_internal(key_code);
			}
		protected:
			virtual ~keyboard() {};
		private:
			virtual void add_listener_internal(key_event_listener const& listener) = 0;
			virtual void process_events_internal() = 0;
			virtual bool is_key_down_internal(int const& key_code) const = 0;
		};

		std::shared_ptr<keyboard> create_keyboard(std::shared_ptr<environment const> const& environment);
	}
}