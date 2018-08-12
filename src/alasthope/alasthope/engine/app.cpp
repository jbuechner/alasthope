#include "pch_allegro.h"
#pragma hdrstop

#include <random>
#include <array>
#include <thread>
#include <future>
#include <atomic>
#include <thread>
#include <chrono>
#include <filesystem>
#include <sstream>
#include <optional>

#include "alasthope/engine/aloo/environment.h"
#include "alasthope/engine/aloo/font.h"
#include "alasthope/engine/aloo/font_manager.h"
#include "alasthope/engine/aloo/keyboard.h"
#include "alasthope/engine/aloo/mouse.h"
#include "alasthope/engine/aloo/display_options.h"
#include "alasthope/engine/aloo/display.h"
#include "alasthope/engine/aloo/drawable.h"
#include "alasthope/engine/aloo/display_loop.h"
#include "alasthope/engine/aloo/render_texture.h"
#include "alasthope/engine/drawables/stage.h"
#include "alasthope/engine/drawables/fps_counter.h"
#include "alasthope/engine/drawables/label.h"
#include "alasthope/engine/drawables/sprite.h"
#include "alasthope/engine/drawables/grid.h"
#include "alasthope/engine/drawables/terrain_sprite_info.h"
#include "alasthope/engine/drawables/structure_sprite_info.h"
#include "alasthope/engine/aloo/utils.h"

#include "structure_info.h"
#include "terrain_info.h"
#include "tile_info.h"
#include "rectangular_grid.h"

#include "app.h"

// as ld42 progresses the code gets uglier and uglier. no time to clean up.

namespace
{
	using namespace engine;
	using namespace engine::aloo;
	using namespace engine::drawables;

	struct date_time
	{
		int32_t year;
		int32_t month;
		int32_t day;
		int32_t hour;
		int32_t minute;

		static constexpr bool is_leap_year(int32_t const& year) noexcept
		{
			return (!(year % 100 == 0) && year % 4 == 0) || (year % 400 == 0);
		}

		static constexpr int32_t get_days_of_month(int32_t const& year, int32_t const& month) noexcept
		{
			if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
			{
				return 31;
			}

			if (month == 4 || month == 6 || month == 9 || month == 11)
			{
				return 30;
			}

			if (month == 2)
			{
				return is_leap_year(year) ? 29 : 28;
			}

			return 0;
		}
	};

	class ressource
	{
	public:
		ressource(std::string const& name, glm::uvec4 const& sprite_source)
			: _name{ name }, _sprite_source{ sprite_source }
		{
		}

		std::string_view const name() const
		{
			return _name;
		}

		glm::uvec4 const& sprite_source() const
		{
			return _sprite_source;
		}
	private:
		std::string const _name{};
		glm::uvec4 const _sprite_source{}; // does not belong here, but there is not time to built a dedicated ressource to sprite map. i need results, not matter how ugly. well i could always fallback to raw C.
	};

	class ressource_flow
	{
	public:
		ressource_flow(ressource const& ressource_)
			: ressource{ ressource_ }
		{
		}

		std::int32_t const& flow_per_second() const
		{
			return _flow_per_second;
		}

		std::int32_t const& storage() const
		{
			return _storage;
		}

		void set_flow_per_second(std::int32_t const& value)
		{
			_flow_per_second = value;
		}

		void add_to_storage(std::int32_t const& value)
		{
			_storage += value;
		}

		ressource const& ressource;
	private:
		std::int32_t _flow_per_second{ 0 };
		std::int32_t _storage{ 0 };
	};

	class ressources
	{
	public:
		static ressource const& water()
		{
			static ressource v{ "water", { 64, 32, 32, 32 } };
			return v;
		}

		static ressource const& electricity()
		{
			static ressource v{ "electricity",{ 32, 32, 32, 32 } };
			return v;
		}

		static ressource const& food()
		{
			static ressource v{ "food", { 96, 32, 32, 32 } };
			return v;
		}

		static ressource const& workforce()
		{
			static ressource v{ "workforce", { 128, 32, 32, 32 } };
			return v;
		}

		static std::vector<std::reference_wrapper<ressource const>> const all()
		{
			static std::vector<std::reference_wrapper<ressource const>> v{ water(), electricity(), food(), workforce() };
			return v;
		}
	};

	enum class day_time_cycle
	{
		night = 0,
		sunrise = 1,
		day = 2,
		sunset = 3
	};

	class backend
	{
	public:
		backend(uint64_t const& map_seed)
			: _map_seed{ map_seed }
		{
			_grid = create_rectangular_grid({ 20, 20 }, map_seed);
			change_time_multiplicator(0);
		}

		date_time get_date_time() const
		{
			return { _years, _months, _days, _hours.count(), _minutes.count() };
		}

		std::shared_ptr<rectangular_grid> const& grid() const
		{
			return _grid;
		}

		uint64_t map_seed() const
		{
			return _map_seed;
		}

		day_time_cycle get_day_time_cycle() const
		{
			if (_hours.count() >= 4 && _hours.count() < 7)
			{
				return day_time_cycle::sunrise;
			}
			if (_hours.count() >= 8 && _hours.count() < 19)
			{
				return day_time_cycle::day;
			}
			if (_hours.count() >= 19 && _hours.count() < 23)
			{
				return day_time_cycle::sunset;
			}

			return day_time_cycle::night;
		}

		void add_to_date_time(int32_t const& years, int32_t const& months, int32_t const& days, std::chrono::hours const& hours, std::chrono::minutes const& minutes)
		{
			_minutes += minutes;
			if (_minutes.count() > 59)
			{
				_hours += std::chrono::hours{ _minutes.count() / 60 };
				_minutes = _minutes % 60;
			}
			_hours += hours;

			int32_t newDays{ days };

			if (_hours.count() > 23)
			{
				newDays += _hours.count() / 24;
				_hours = std::chrono::hours{ _hours.count() % 24 };
			}

			int32_t max_days{ date_time::get_days_of_month(_years, _months) };

			if (newDays > 0)
			{
				do
				{
					int32_t remainingDays = max_days - _days;
					int32_t diff = std::max(0, std::min(remainingDays, newDays));
					_days += diff;
					newDays -= diff;
					if (newDays > 0)
					{
						_months += 1;
						_days = 1;
						if (_months > 12)
						{
							_years += 1;
							_months = 1;
						}
					}
					max_days = date_time::get_days_of_month(_years, _months);

					if (max_days == 0)
					{
						_ASSERT(false);
						break;
					}
				} while (newDays > 0);
			}

			if (_days > max_days)
			{
				_months += 1;
				_days = _days % max_days;
			}
			
			_months += months;
			if (_months > 12)
			{
				_years += _months / 12;
				_months = (_months % 12) + 1;
			}
			_years += years;
		}

		std::string get_date_time_text() const
		{
			std::stringstream stream{};
			stream << std::setfill('0') << _years << "-" << std::setw(2) << static_cast<int32_t>(_months) << "-" << std::setw(2) << static_cast<int32_t>(_days) << " " << std::setw(2) << _hours.count() << ":" << std::setw(2) << _minutes.count();
			return std::move(stream.str());
		}

		std::shared_ptr<ressource_flow> const& water_flow()
		{
			return _water;
		}

		std::shared_ptr<ressource_flow> const& electricity_flow()
		{
			return _electricity;
		}

		std::shared_ptr<ressource_flow> const& food_flow()
		{
			return _food;

		}

		std::shared_ptr<ressource_flow> const& workforce_flow()
		{
			return _workforce;
		}

		double temperature() const
		{
			return _temperature;
		}

		void change_temperature(double const& change)
		{
			_temperature = change;
		}

		size_t time_multiplicator() const
		{
			return _time_multiplicator;
		}

		void change_time_multiplicator(int32_t const& change)
		{
			static std::vector<size_t> values{ 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
			int32_t newValue = static_cast<int32_t>(_time_multiplicator_index) + change;
			newValue = std::max(1, std::min(static_cast<int32_t>(values.size()), newValue));
			_time_multiplicator_index = newValue;
			_time_multiplicator = values.at(newValue - 1);
		}

		void tick()
		{
			add_to_date_time(0, 0, 0, std::chrono::hours{ 0 }, _tick_duration);

			switch (get_day_time_cycle())
			{
			case day_time_cycle::sunrise:
			{
				change_temperature(0.02);
				break;
			}
			case day_time_cycle::day:
			{
				change_temperature(0.04);
				break;
			}
			case day_time_cycle::sunset:
			{
				change_temperature(0.01);
				break;
			}
			case day_time_cycle::night:
			{
				change_temperature(-0.03);
				break;
			}
			}
		}
	private:
		double _temperature{ 0 };
		int32_t _years { 2109 };
		int32_t _months { 6 };
		int32_t _days { 3 };
		std::chrono::hours _hours{ 0 };
		std::chrono::minutes _minutes{ 0 };
		std::shared_ptr<rectangular_grid> _grid{ nullptr };
		uint64_t const _map_seed;

		size_t _time_multiplicator_index{ 1 };
		size_t _time_multiplicator{ 0 };

		std::chrono::minutes _tick_duration{ 1 };

		std::shared_ptr<ressource_flow> _water { std::make_shared<ressource_flow>(ressources::water()) };
		std::shared_ptr<ressource_flow> _electricity{ std::make_shared<ressource_flow>(ressources::electricity()) };
		std::shared_ptr<ressource_flow> _food{ std::make_shared<ressource_flow>(ressources::food()) };
		std::shared_ptr<ressource_flow> _workforce{ std::make_shared<ressource_flow>(ressources::workforce()) };
	};

	class game_loop
	{
	public:
		game_loop()
		{
			set_sleep_duration(default_sleep_duration());
		}

		static constexpr std::chrono::nanoseconds default_sleep_duration() noexcept
		{
			using namespace std::chrono_literals;
			return 1s;
		}

		void set_time_multiplicator(size_t const& value)
		{
			using namespace std::chrono_literals;

			auto const new_duration = default_sleep_duration() / value;
			if (new_duration > 10s || new_duration < 0s)
			{
				set_sleep_duration(default_sleep_duration());
			}
			else
			{
				set_sleep_duration(new_duration);
			}
		}

		template <typename repT, typename periodT = std::chrono::high_resolution_clock::rep>
		void set_sleep_duration(std::chrono::duration<repT, periodT> const& duration)
		{
			_sleep_duration = std::chrono::duration_cast<decltype(_sleep_duration)>(duration);
		}

		void set_backend(std::shared_ptr<backend> const& backend)
		{
			std::unique_lock<std::mutex> lock{ _mtx };
			_backend = backend;
		}

		void operator()(std::atomic<bool> const& do_continue)
		{
			using namespace std::chrono_literals;

			while (do_continue)
			{
				std::this_thread::sleep_for(_sleep_duration);

				std::lock_guard<std::mutex> lock{ _mtx };
				if (_backend)
				{
					_backend->tick();
				}
			}
		}
	private:
		std::mutex _mtx{};
		std::shared_ptr<backend> _backend{ nullptr };
		std::chrono::nanoseconds _sleep_duration{ 0 };
	};

	class weather_info_drawable : public drawable
	{
	public:
		weather_info_drawable(std::shared_ptr<font> const& small_font, std::shared_ptr<font> const& big_font, std::shared_ptr<render_texture> const& spritesheet)
			: _font{ small_font }, _font2{ big_font }, _spritesheet{ spritesheet }
		{
		}

		void set_backend(std::shared_ptr<backend const> const& backend)
		{
			_backend = backend;
			if (_backend)
			{
				_current_cycle = _backend->get_day_time_cycle();
				update_current_cycle();
			}
		}
	private:
		void update_current_cycle()
		{
			switch (_current_cycle)
			{
			case day_time_cycle::night:
			{
				_current_cycle_text = "night";
				_source_location = { 128, 64 };
				break;
			}
			case day_time_cycle::sunrise:
			{
				_current_cycle_text = "sunrise";
				_source_location = { 32, 64 };
				break;
			}
			case day_time_cycle::day:
			{
				_current_cycle_text = "day";
				_source_location = { 96, 64 };
				break;
			}
			case day_time_cycle::sunset:
			{
				_current_cycle_text = "sunset";
				_source_location = { 64, 64 };
				break;
			}
			}
		}

		void set_position_internal(glm::ivec2 const& position) override
		{
			_position = position;
		}

		bool draw_requested_internal(draw_context&) const override
		{
			return _backend != nullptr;
		}

		void draw_internal(draw_context&) override
		{
			auto new_day_time_cycle = _backend->get_day_time_cycle();
			if (new_day_time_cycle != _current_cycle)
			{
				_current_cycle = new_day_time_cycle;
				update_current_cycle();
			}

			al_draw_bitmap_region(reinterpret_cast<ALLEGRO_BITMAP*>(_spritesheet->get_native_ptr()), _source_location.x, _source_location.y, 32, 16, _position.x, _position.y, 0);
			al_draw_text(reinterpret_cast<ALLEGRO_FONT*>(_font->get_native_ptr()), _color, _position.x + 16, _position.y + 18, allegro_draw_text_flags() | ALLEGRO_ALIGN_CENTER, _current_cycle_text.c_str());

			al_draw_text(reinterpret_cast<ALLEGRO_FONT*>(_font2->get_native_ptr()), _color, _position.x + 48, _position.y + 4, allegro_draw_text_flags(), "100 °C");
		}

		day_time_cycle _current_cycle;
		glm::uvec2 _source_location{ };
		std::string _current_cycle_text{};
		std::array<char, std::numeric_limits<uint64_t>::digits10 + 1> _textBuffer{};
		ALLEGRO_COLOR _color{ al_map_rgb(0x22, 0x22, 0x22) };
		glm::ivec2 _position{};
		std::shared_ptr<font> const _font;
		std::shared_ptr<font> const _font2;
		std::shared_ptr<render_texture> const _spritesheet;
		std::shared_ptr<backend const> _backend{ nullptr };
	};

	class ressource_info_drawable : public drawable
	{
	public:
		ressource_info_drawable(std::shared_ptr<font> const& small_font, std::shared_ptr<font> const& font, std::shared_ptr<render_texture> const& spritesheet, ressource const& ressource)
			: _font{ small_font }, _font2{ font }, _spritesheet{ spritesheet }, _ressource{ ressource }, _sprite_source{ _ressource.sprite_source() }
		{
		}

		void set_flow(std::shared_ptr<ressource_flow const> const& flow)
		{
			_flow = flow;
		}
	private:
		void set_position_internal(glm::ivec2 const& position) override
		{
			_position = position;
		}

		bool draw_requested_internal(draw_context&) const override
		{
			return _flow != nullptr;
		}

		void draw_internal(draw_context&) override
		{
			al_draw_bitmap_region(reinterpret_cast<ALLEGRO_BITMAP*>(_spritesheet->get_native_ptr()), _sprite_source.x, _sprite_source.y, _sprite_source.z, _sprite_source.w, _position.x, _position.y, 0);
			al_draw_text(reinterpret_cast<ALLEGRO_FONT*>(_font->get_native_ptr()), _color, _position.x + 16, _position.y + 32, allegro_draw_text_flags() | ALLEGRO_ALIGN_CENTER, &_ressource.name()[0]);

			ALLEGRO_FONT* font_ptr = reinterpret_cast<ALLEGRO_FONT*>(_font2->get_native_ptr());
			to_string(_flow->storage(), &_textBuffer[1]);
			_textBuffer[0] = ' ';
			al_draw_text(font_ptr, _color, _position.x + 64, _position.y + 4, allegro_draw_text_flags(), &_textBuffer[0]);

			auto flow_per_second = _flow->flow_per_second();
			to_string(flow_per_second, &_textBuffer[1]);
			_textBuffer[0] = flow_per_second < 0 ? '-' : '+';
			al_draw_text(font_ptr, flow_per_second < 0 ? _red : _green, _position.x + 64, _position.y + 24, allegro_draw_text_flags(), &_textBuffer[0]);
		}

		std::array<char, std::numeric_limits<uint64_t>::digits10 + 1> _textBuffer{};
		ALLEGRO_COLOR _color{ al_map_rgb(0x22, 0x22, 0x22) };
		ALLEGRO_COLOR _red{ al_map_rgb(0x88, 0x44, 0x44) };
		ALLEGRO_COLOR _green{ al_map_rgb(0x44, 0x88, 0x44) };
		glm::ivec2 _position{};
		std::shared_ptr<font> const _font;
		std::shared_ptr<font> const _font2;
		std::shared_ptr<render_texture> const _spritesheet;
		ressource const& _ressource;
		std::shared_ptr<ressource_flow const> _flow{ nullptr };
		glm::uvec4 const _sprite_source;
	};

	class game_info_overlay_drawable : public drawable
	{
	public:
		enum class state
		{
			none,
			start_game
		};

		game_info_overlay_drawable(std::shared_ptr<font> const& font, std::shared_ptr<display const> const& display)
			: _font{ font }, _display{ display }
		{
			_center = display->display_size() / glm::uvec2 { 2, 2 };
		}

		void set_state(state const& value)
		{
			_state = value;
		}

	private:
		bool draw_requested_internal(draw_context&) const
		{
			return _state != state::none;
		}

		void draw_internal(draw_context&) override
		{
			switch (_state)
			{
			case state::start_game:
				al_draw_multiline_text(reinterpret_cast<ALLEGRO_FONT*>(_font->get_native_ptr()), _color, _center.x, _center.y - 24, 650, 24, allegro_draw_text_flags() | ALLEGRO_ALIGN_CENTER, "Press [N] to start new game. You can also press [N] during the game to restart with another seed.");
				break;
			}
		}

		ALLEGRO_COLOR _color{ al_map_rgb(0x22, 0x22, 0x22) };
		glm::uvec2 _center{};
		state _state{ state::none };
		std::shared_ptr<font> const _font;
		std::shared_ptr<display const> const _display;
	};

	class app_internal : public engine::app
	{
	public:
		app_internal()
		{
			_keyboard = std::move(create_keyboard(_environment));
			_mouse = std::move(create_mouse(_environment));
			_display = std::move(create_display(_environment, { {}, { 1280, 720 } }));

			setup();
		}

	private:
		void setup()
		{
			auto& display = *_display;
			_loop = display.create_loop();

			auto const defaultFont = _environment->get_font_manager()->get_font(std::filesystem::path{ "gfx" } / std::filesystem::path{ "hack-regular.ttf" }, 16);
			auto const ressourceFont = _environment->get_font_manager()->get_font(std::filesystem::path{ "gfx" } / std::filesystem::path{ "hack-regular.ttf" }, 20);
			auto const bigFont = _environment->get_font_manager()->get_font(std::filesystem::path{ "gfx" } / std::filesystem::path{ "hack-regular.ttf" }, 24);
			auto const smallFont = _environment->get_font_manager()->get_font(std::filesystem::path{ "gfx" } / std::filesystem::path{ "hack-regular.ttf" }, 12);
			auto const spritesheet{ create_render_texture(std::filesystem::path{ "gfx" } / std::filesystem::path{ "spritesheet.png" }) };

			_drawable_grid = drawables::create_grid({ spritesheet });
			_drawable_grid->set_position(_display->backbuffer_size() / glm::uvec2{ 2, 2 });

			_tile_info_coordinate_label = create_label();
			_tile_info_coordinate_label->set_position({ 48, 400 });
			_tile_info_coordinate_label->set_font(defaultFont);
			_tile_info_coordinate_label->center_text();

			_tile_info_sprite = create_sprite();
			_tile_info_sprite->set_position({ 32, 432 });
			_tile_info_sprite->set_render_texture(spritesheet);

			_tile_info_terrain_label = create_label();
			_tile_info_terrain_label->set_position(_tile_info_sprite->position() + glm::ivec2{ 42, 8 });
			_tile_info_terrain_label->set_font(defaultFont);

			_structure_info_sprite = create_sprite();
			_structure_info_sprite->set_position(_tile_info_sprite->position() + glm::ivec2{ 0, 64 });
			_structure_info_sprite->set_render_texture(spritesheet);

			_structure_info_label = create_label();
			_structure_info_label->set_position(_structure_info_sprite->position() + glm::ivec2{ 42, 8 });
			_structure_info_label->set_font(defaultFont);

			_datetime_label = create_label();
			_datetime_label->set_position({ 32, 50 });
			_datetime_label->set_font(bigFont);

			_time_multiplicator_label = create_label();
			_time_multiplicator_label->set_position({ 32, 72 });
			_time_multiplicator_label->set_font(smallFont);

			auto stage = create_stage();
			stage->append(_drawable_grid);

			glm::uvec2 resource_icon_offset{ 32, 64 };
			glm::uvec2 resource_icon_margin{ 0, 64 };

			auto const add_ui_border = [&](glm::uvec2 const& position)
			{
				auto sprite = create_sprite();
				sprite->set_render_texture(spritesheet);
				sprite->set_position(position);
				sprite->set_source_region({ 76, 187, 136, 57 });
				_loop->append(sprite);
			};

			_water_flow_drawable = std::make_shared<ressource_info_drawable>(smallFont, ressourceFont, spritesheet, ressources::water());
			_water_flow_drawable->set_position(resource_icon_offset += resource_icon_margin);

			_electricity_flow_drawable = std::make_shared<ressource_info_drawable>(smallFont, ressourceFont, spritesheet, ressources::electricity());
			_electricity_flow_drawable->set_position(resource_icon_offset += resource_icon_margin);

			_food_flow_drawable = std::make_shared<ressource_info_drawable>(smallFont, ressourceFont, spritesheet, ressources::food());
			_food_flow_drawable->set_position(resource_icon_offset += resource_icon_margin);

			_workforce_flow_drawable = std::make_shared<ressource_info_drawable>(smallFont, ressourceFont, spritesheet, ressources::workforce());
			_workforce_flow_drawable->set_position(resource_icon_offset += resource_icon_margin);

			_weather_info_drawable = std::make_shared<weather_info_drawable>(smallFont, bigFont, spritesheet);
			_weather_info_drawable->set_position({ 32, 92 });

			_loop->append(_water_flow_drawable);
			_loop->append(_electricity_flow_drawable);
			_loop->append(_food_flow_drawable);
			_loop->append(_workforce_flow_drawable);
			_loop->append(_weather_info_drawable);

			_game_info_overlay = std::make_shared<game_info_overlay_drawable>(bigFont, _display);

			_loop->append(stage);
			_loop->append(_tile_info_sprite);
			_loop->append(_tile_info_coordinate_label);
			_loop->append(_tile_info_terrain_label);
			_loop->append(_structure_info_sprite);
			_loop->append(_structure_info_label);
			_loop->append(_datetime_label);
			_loop->append(_time_multiplicator_label);
			_loop->append(_game_info_overlay);
			_loop->append(create_fps_counter(defaultFont));

			_game_info_overlay->set_state(game_info_overlay_drawable::state::start_game);

			_keyboard->add_listener([&](int const& key_code, key_state const& key_state)
			{

				if (key_state == key_state::pressed)
				{
					if (key_code == ALLEGRO_KEY_PAD_PLUS || key_code == ALLEGRO_KEY_EQUALS)
					{
						if (!_backend)
						{
							return;
						}
						_backend->change_time_multiplicator(1);
						update_time_multiplicator();
					}
					if (key_code == ALLEGRO_KEY_PAD_MINUS || key_code == ALLEGRO_KEY_MINUS)
					{
						if (!_backend)
						{
							return;
						}
						_backend->change_time_multiplicator(-1);
						update_time_multiplicator();
					}
					if (key_code == ALLEGRO_KEY_N)
					{
						start_new_game();
					}
				}
			});
		}

		void start_new_game()
		{
			_game_info_overlay->set_state(game_info_overlay_drawable::state::none);

#if _DEBUG
			uint64_t const map_seed{ 0 };
#else
			uint64_t const map_seed{ std::chrono::system_clock::now().time_since_epoch().count() };
#endif

			_backend = std::make_shared<backend>( map_seed );
			_game_loop.set_backend(_backend);
			_drawable_grid->set_grid(_backend->grid());

			_water_flow_drawable->set_flow(_backend->water_flow());
			_electricity_flow_drawable->set_flow(_backend->electricity_flow());
			_food_flow_drawable->set_flow(_backend->food_flow());
			_workforce_flow_drawable->set_flow(_backend->workforce_flow());
			_weather_info_drawable->set_backend(_backend);

			update_time_multiplicator();
		}

		void run_and_wait_internal() override
		{
			auto display_to_backbuffer_factor = static_cast<glm::vec2>(_display->backbuffer_size()) / static_cast<glm::vec2>(_display->display_size());
			auto gameLoopFuture = std::async(std::launch::async, [&]() { _game_loop(_continueRendering); });

			while ((*_loop)() && _continueRendering.load())
			{
				_keyboard->process_events();
				_mouse->process_events();

				if (_mouse->cursor_position_changed())
				{
					auto const position = static_cast<glm::uvec2>(static_cast<glm::vec2>(_mouse->position()) * display_to_backbuffer_factor);

					auto const tileCoordinate = _drawable_grid->get_coordinate_at_mouse_position(position);
					update_highlighted_tile(tileCoordinate);
					if (tileCoordinate)
					{
						_drawable_grid->set_cursor_position(*tileCoordinate);
						_drawable_grid->set_cursor_visible(true);
					}
					else
					{
						_drawable_grid->set_cursor_visible(false);
					}
				}

				if (_backend)
				{
					_datetime_label->set_text(_backend->get_date_time_text());
				}
				else
				{
					_datetime_label->set_text("");
				}

				// todo: less stupid key handling required
				if (_keyboard->is_key_down(ALLEGRO_KEY_ESCAPE))
				{
					_continueRendering.store(false);
				}
			}

			gameLoopFuture.wait_for(std::chrono::milliseconds{ 100 });
		}

		void update_time_multiplicator()
		{
			if (!_backend)
			{
				return;
			}

			size_t const value = _backend->time_multiplicator();
			_game_loop.set_time_multiplicator(value);
			std::array<char, std::numeric_limits<size_t>::digits10> buffer{};
			to_string(static_cast<int32_t>(value), buffer);
			_time_multiplicator_label->set_text("x" + std::string { &buffer[0] });
		}

		void update_highlighted_tile(std::optional<glm::uvec2 const> const& tile_coordinate)
		{
			if (!_tile_info_coordinate_label)
			{
				return;
			}

			bool const visible{ tile_coordinate };

			if (tile_coordinate && _backend)
			{
				auto tile_info = _backend->grid()->lookup(*tile_coordinate);
				std::stringstream buffer{};
				buffer << (*tile_coordinate).x << ", " << (*tile_coordinate).y;

				_tile_info_sprite->set_source_region({ 0, 0, 32, 32 });
				_tile_info_terrain_label->set_text(std::string{ tile_info->terrain().name });
				_tile_info_coordinate_label->set_text(buffer.str());

				auto const& terrain_sprite_info = lookup_terrain_sprite_info(tile_info->terrain().id, tile_info->terrain_variation());
				_tile_info_sprite->set_source_region({ terrain_sprite_info.source, 32, 32 });

				auto const& structure_sprite_info = lookup_structure_sprite_info(tile_info->structure().id);
				_structure_info_sprite->set_source_region({ structure_sprite_info.source, 32, 32 });
				_structure_info_label->set_text(std::string{ tile_info->structure().name });
			}

			_tile_info_terrain_label->set_visible(visible);
			_tile_info_coordinate_label->set_visible(visible);
			_tile_info_sprite->set_visible(visible);
			_structure_info_sprite->set_visible(visible);
			_structure_info_label->set_visible(visible);
		}

		std::shared_ptr<backend> _backend{ nullptr };
		game_loop _game_loop{};

		mutable std::atomic<bool> _continueRendering { true };
		std::shared_ptr<environment> const _environment{ std::move(create_environment()) };
		std::shared_ptr<display> _display{ nullptr };
		std::shared_ptr<display_loop> _loop{ nullptr };
		std::shared_ptr<mouse> _mouse{ nullptr };
		std::shared_ptr<keyboard> _keyboard{ nullptr };

		std::shared_ptr<game_info_overlay_drawable> _game_info_overlay{ nullptr };

		std::shared_ptr<ressource_info_drawable> _water_flow_drawable{ nullptr };
		std::shared_ptr<ressource_info_drawable> _electricity_flow_drawable{ nullptr };
		std::shared_ptr<ressource_info_drawable> _food_flow_drawable{ nullptr };
		std::shared_ptr<ressource_info_drawable> _workforce_flow_drawable{ nullptr };

		std::shared_ptr<weather_info_drawable> _weather_info_drawable{ nullptr };

		std::shared_ptr<sprite> _tile_info_sprite{ nullptr };
		std::shared_ptr<label> _tile_info_coordinate_label { nullptr };
		std::shared_ptr<label> _tile_info_terrain_label{ nullptr };

		std::shared_ptr<sprite> _structure_info_sprite{ nullptr };
		std::shared_ptr<label> _structure_info_label{ nullptr };

		std::shared_ptr<label> _datetime_label{ nullptr };
		std::shared_ptr<label> _time_multiplicator_label{ nullptr };

		std::shared_ptr<grid> _drawable_grid{ nullptr };
	};
}

namespace engine
{
	std::shared_ptr<app> create_app()
	{
		return std::move(std::make_shared<app_internal>());
	}
}