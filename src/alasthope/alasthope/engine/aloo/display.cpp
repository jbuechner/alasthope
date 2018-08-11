#include "pch_allegro.h"
#pragma hdrstop

#include <thread>
#include <array>
#include <memory>
#include <chrono>
#include <filesystem>
#include <mutex>

#include "utils.h"
#include "environment.h"
#include "display_options.h"
#include "display.h"
#include "display_loop.h"
#include "font.h"
#include "timer.h"
#include "fps_component.h"
#include "render_texture.h"
#include "hierarchical.h"

#include "draw_context.h"
#include "drawable.h"

namespace
{
	using namespace std::chrono_literals;
	using namespace engine::aloo;

	class display_internal : public engine::aloo::display, public std::enable_shared_from_this<display_internal>
	{
	public:
		display_internal(std::shared_ptr<environment> const& environment, display_options const& options)
			: _environment{ environment }, _display_size{ options.display_size }, _backbuffer_size{ options.backbuffer_size }
		{
			al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_OPENGL_3_0 | ALLEGRO_OPENGL_FORWARD_COMPATIBLE);

			al_reset_new_display_options();
			al_set_new_display_option(ALLEGRO_VSYNC, _use_vsync ? 1 : 0, ALLEGRO_SUGGEST);
			al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
			al_set_new_display_option(ALLEGRO_SAMPLES, 2, ALLEGRO_SUGGEST);

			if (_display_size == glm::uvec2{})
			{
				bool found{ false };
				int size = al_get_num_video_adapters();
				ALLEGRO_MONITOR_INFO info{};
				for (int i = 0; i < size; i++)
				{
					if (al_get_monitor_info(i, &info) && info.x1 == info.y1 == 0)
					{
						_display_size = { info.x2 - info.x1, info.y2 - info.y1 };
						al_set_new_display_flags(al_get_new_display_flags() | ALLEGRO_FRAMELESS);
						found = true;
					}
				}
				if (!found)
				{
					_display_size = _backbuffer_size;
				}
			}

			_display = al_display{ al_create_display(_display_size.x, _display_size.y) };
			_backbuffer_texture = create_render_texture(_backbuffer_size.x, _backbuffer_size.y, render_texture_flags::video, render_texture_format::best);
		}

		void close()
		{
			_is_closed = true;
		}

		ALLEGRO_DISPLAY* get_native_ptr() const
		{
			return _display.get();
		}

		size_t target_fps() const
		{
			return _target_fps;
		}

		bool uses_vsync() const
		{
			return _use_vsync;
		}

		bool is_closed() const
		{
			return _is_closed;
		}
	private:
		std::shared_ptr<engine::aloo::display_loop> create_loop_internal() override
		{
			return create_allegro_loop_internal<>();
		}

		void get_size_internal(glm::uvec2& value) const override
		{
			auto* ptr = _display.get();
			value.x = al_get_display_width(ptr);
			value.y = al_get_display_height(ptr);
		}

		render_texture& get_backbuffer_internal() const override
		{
			return *_backbuffer_texture;
		}

		bool backbuffer_and_frontbuffer_are_perfect_internal() const override
		{
			return _are_backbuffer_and_frontbuffer_are_perfect;
		}

		glm::uvec2 const& display_size_internal() const override
		{
			return _display_size;
		}

		glm::uvec2 const& backbuffer_size_internal() const override
		{
			return _backbuffer_size;
		}

		template <bool placeholder = true>
		std::shared_ptr<engine::aloo::display_loop> create_allegro_loop_internal()
		{
			return std::move(std::make_shared<display_loop_internal>(shared_from_this()));
		}

		// todo: configuration
		glm::uvec2 _display_size;
		glm::uvec2 _backbuffer_size;

		bool _are_backbuffer_and_frontbuffer_are_perfect{ false };

		size_t _target_fps{ 30 };
		bool _use_vsync{ true };

		std::shared_ptr<render_texture> _backbuffer_texture;
		std::shared_ptr<engine::aloo::environment> const _environment;
		al_display _display{ nullptr };
		bool _is_closed{ false };
	};

	class display_event_loop
	{
	public:
		display_event_loop(std::shared_ptr<display_internal> const& display)
			: _display{ display }
		{
			auto* displayEventSource = al_get_display_event_source(display->get_native_ptr());
			al_register_event_source(_event_queue.get(), displayEventSource);
		}

		void process()
		{
			while (al_get_next_event(_event_queue.get(), &_ev))
			{
				switch (_ev.type)
				{
				case ALLEGRO_EVENT_DISPLAY_CLOSE:
					_display->close();
					break;
				}
			}
		}
	private:
		ALLEGRO_EVENT _ev{};
		al_event_queue _event_queue{ al_create_event_queue() };
		std::shared_ptr<display_internal> const _display;
	};

	class display_loop_internal : public engine::aloo::display_loop
	{
	public:
		display_loop_internal(std::shared_ptr<display_internal> const& display)
			: _display{ display }, _display_event_loop{ display }, _draw_context{ display }
		{
			invalidate();
		}

		bool operator()()
		{
			_timer->wait_for_tick();
			_draw_context.fps_measure_result() = _fps.measure();
			_draw_context.fps_as_string() = _fps.text();

			_display_event_loop.process();

			al_clear_to_color(_clear_color);

			for (auto const& drawable : _children)
			{
				if (drawable)
				{
					if (drawable->draw_requested(_draw_context))
					{

						drawable->draw(_draw_context);

					}
				}
			}

			al_flip_display();

			return !_display->is_closed();
		}
	private:
		std::list<std::shared_ptr<drawable>> const& children_internal() const override
		{
			return _children;
		}

		void append_internal(std::shared_ptr<drawable> const& drawable) override
		{
			_children.push_back(drawable);
		}

		void invalidate()
		{
			auto const fpsStepTimeInMs = std::chrono::milliseconds { static_cast<std::chrono::milliseconds::rep>((1.0 / static_cast<double>(_display->target_fps())) * 1000.0) };
			_timer = create_timer(fpsStepTimeInMs);
		}

		draw_context _draw_context;
		fps_component _fps{};

		ALLEGRO_COLOR const _clear_color{ al_map_rgb(0xee, 0xc3, 0x9a) };

		std::list<std::shared_ptr<drawable>> _children{};
		std::shared_ptr<timer> _timer{ nullptr };
		std::shared_ptr<display_internal> const _display;
		display_event_loop _display_event_loop;
	};
}

namespace engine
{
	namespace aloo
	{
		std::shared_ptr<display> create_display(std::shared_ptr<environment> const& environment, display_options const& options)
		{
			return std::move(std::make_shared<display_internal>(environment, options));
		}
	}
}