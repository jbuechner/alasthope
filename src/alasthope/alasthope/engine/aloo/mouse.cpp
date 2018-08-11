#include "pch_allegro.h"
#pragma hdrstop

#include "environment.h"
#include "utils.h"
#include "mouse.h"

namespace
{
	using namespace engine::aloo;

	class mouse_internal : public mouse
	{
	public:
		~mouse_internal()
		{
			al_uninstall_mouse();
		}

		mouse_internal(std::shared_ptr<environment const> const& environment)
			: _environment{ environment }
		{
			al_system<&al_install_mouse>{"unable to install mouse sub system."};

			_event_queue = al_create_event_queue();
			auto* event_source = al_get_mouse_event_source();
			al_register_event_source(_event_queue.get(), event_source);
		}
	private:
		void process_events_internal() override
		{
			while (al_get_next_event(_event_queue.get(), &_ev))
			{
			}

			glm::ivec2 position{};
			al_get_mouse_cursor_position(&position.x, &position.y);
			if (position != _position)
			{
				_has_position_changed = true;
				_position = position;
			}
		}

		bool cursor_position_changed_internal() const override
		{
			return _has_position_changed;
		}

		glm::uvec2 position_internal() const override
		{
			return _position;
		}

		bool _has_position_changed{ false };
		glm::ivec2 _position;
		al_event_queue _event_queue{ nullptr };
		ALLEGRO_EVENT _ev;

		std::shared_ptr<environment const> const _environment;
	};
}

namespace engine
{
	namespace aloo
	{
		std::shared_ptr<mouse> create_mouse(std::shared_ptr<environment const> const& environment)
		{
			return std::make_shared<mouse_internal>(environment);
		}
	}
}