#include "pch_allegro.h"
#pragma hdrstop

#include <map>

#include "utils.h"
#include "keyboard.h"

namespace
{
	using namespace engine::aloo;

	// todo: not thread safe
	class keyboard_internal : public keyboard
	{
	public:
		~keyboard_internal()
		{
			al_uninstall_keyboard();
		}

		keyboard_internal(std::shared_ptr<environment const> const& environment)
			: _environment{ environment }
		{
			al_system<&al_install_keyboard>{"unable to install keyboard sub system."};

			_event_queue = std::move(al_event_queue{ al_create_event_queue() });
			auto* eventSource = al_get_keyboard_event_source();
			al_register_event_source(_event_queue.get(), eventSource);
		}
	private:
		static constexpr key_state map_to_key_state(int const& keyEventType) noexcept
		{
			if (keyEventType == ALLEGRO_EVENT_KEY_DOWN)
			{
				return key_state::pressed;
			}

			return key_state::none;
		}

		void handle_keyboard_event(ALLEGRO_EVENT const& ev)
		{
			auto const keyState{ map_to_key_state(ev.keyboard.type) };
			_key_states.insert_or_assign(ev.keyboard.keycode, keyState);
		}

		void process_events_internal()
		{
			ALLEGRO_EVENT ev;
			while (al_get_next_event(_event_queue.get(), &ev))
			{
				switch (ev.type)
				{
				case ALLEGRO_EVENT_KEY_DOWN:
				case ALLEGRO_EVENT_KEY_UP:
				{
					handle_keyboard_event(ev);
					break;
				}
				}
			}
		}

		bool is_key_down_internal(int const& key_code) const
		{
			auto const iter = _key_states.find(key_code);
			if (iter == _key_states.cend())
			{
				return false;
			}
			return (*iter).second == key_state::pressed;
		}

		std::map<int, key_state> _key_states{};
		al_event_queue _event_queue{ nullptr };
		std::shared_ptr<environment const> const _environment;
	};
}

namespace engine
{
	namespace aloo
	{
		std::shared_ptr<keyboard> create_keyboard(std::shared_ptr<environment const> const& environment)
		{
			return std::make_shared<keyboard_internal>(environment);
		}
	}
}