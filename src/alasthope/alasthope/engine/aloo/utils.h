#pragma once

namespace engine
{
	namespace aloo
	{
		template <bool init()>
		class al_system
		{
		public:
			al_system(std::string const& initialization_error_msg)
			{
				if (!init())
				{
					throw std::exception{ initialization_error_msg.c_str() };
				}
			}
		};

		template <typename T, void deleter(T*)>
		class guarded_allegro_object
		{
		public:
			~guarded_allegro_object()
			{
				if (_value)
				{
					deleter(_value);
				}
			}

			guarded_allegro_object(T* value)
				: _value{ value }
			{
			}

			guarded_allegro_object(guarded_allegro_object&& other)
				: _value{ other._value }
			{
				other._value = nullptr;
			}

			guarded_allegro_object(guarded_allegro_object const&) = delete;


			guarded_allegro_object* operator=(guarded_allegro_object&& other)
			{
				_value = other._value;
				other._value = nullptr;
				return this;
			}

			guarded_allegro_object* operator=(guarded_allegro_object const& other) = delete;

			void release()
			{
				_value = nullptr;
			}

			inline T* get() const
			{
				return _value;
			}

			operator bool() const
			{
				return _value != nullptr;
			}

		private:
			T * _value{ nullptr };
		};

		using al_event_queue = engine::aloo::guarded_allegro_object<ALLEGRO_EVENT_QUEUE, &al_destroy_event_queue>;
		using al_display = engine::aloo::guarded_allegro_object<ALLEGRO_DISPLAY, &al_destroy_display>;
		using al_font = engine::aloo::guarded_allegro_object<ALLEGRO_FONT, &al_destroy_font>;
		using al_timer = engine::aloo::guarded_allegro_object<ALLEGRO_TIMER, &al_destroy_timer>;
		using al_bitmap = guarded_allegro_object<ALLEGRO_BITMAP, &al_destroy_bitmap>;

		void to_string(int32_t const& value, char* const buffer);

		template <typename bufferT>
		void to_string(int32_t const& value, bufferT const& buffer)
		{
			to_string(value, &buffer[0]);
		}

		constexpr auto allegro_draw_text_flags()
		{
			return ALLEGRO_ALIGN_LEFT | ALLEGRO_ALIGN_INTEGER;
		}
	}
}