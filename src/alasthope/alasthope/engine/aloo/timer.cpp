#include "pch_allegro.h"
#pragma hdrstop

#include "utils.h"
#include "timer.h"

namespace
{
	using namespace engine::aloo;

	class timer_internal : public timer
	{
	public:
		timer_internal(std::chrono::milliseconds const& fpsStepTime)
			: _timer { al_create_timer(fpsStepTime.count() / 1000.0) },
			_event_queue{ al_create_event_queue() }
		{
			auto* timerEventSource = al_get_timer_event_source(_timer.get());
			al_register_event_source(_event_queue.get(), timerEventSource);
			al_start_timer(_timer.get());
		}

	private:
		void wait_for_tick_internal() const override
		{
			al_wait_for_event(_event_queue.get(), &_ev);
			al_flush_event_queue(_event_queue.get());
		}

		al_timer const _timer { nullptr };
		al_event_queue _event_queue{ nullptr };

		mutable ALLEGRO_EVENT _ev;
	};
}

namespace engine
{
	namespace aloo
	{
		std::shared_ptr<timer> create_timer(std::chrono::milliseconds const& fpsStepTime)
		{
			return std::make_shared<timer_internal>(fpsStepTime);
		}
	}
}