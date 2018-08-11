#include "pch_allegro.h"
#pragma hdrstop

#include <atomic>
#include <thread>
#include <chrono>
#include <filesystem>
#include <sstream>
#include <optional>

#include "alasthope/engine/aloo/environment.h"
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

#include "terrain_info.h"
#include "tile_info.h"
#include "rectangular_grid.h"

#include "app.h"

namespace
{
	using namespace engine;
	using namespace engine::aloo;
	using namespace engine::drawables;

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
			auto const smallFont = _environment->get_font_manager()->get_font(std::filesystem::path{ "gfx" } / std::filesystem::path{ "hack-regular.ttf" }, 12);
			auto const spritesheet{ create_render_texture(std::filesystem::path{ "gfx" } / std::filesystem::path{ "spritesheet.png" }) };

			_drawable_grid = drawables::create_grid({ spritesheet }, _grid);
			_tile_info_coordinate_label = create_label();
			_tile_info_coordinate_label->set_position({ 48, 108 });
			_tile_info_coordinate_label->set_font(defaultFont);
			_tile_info_coordinate_label->center_text();

			_tile_info_sprite = create_sprite();
			_tile_info_sprite->set_position({ 32, 70 });
			_tile_info_sprite->set_render_texture(spritesheet);

			_tile_info_terrain_label = create_label();
			_tile_info_terrain_label->set_position(_tile_info_sprite->position() + glm::ivec2{ 42, 8 });
			_tile_info_terrain_label->set_font(defaultFont);

			auto stage = create_stage();
			stage->append(_drawable_grid);

			glm::uvec2 resource_icon_offset{ 32, 120 };
			glm::uvec2 resource_icon_margin{ 0, 46 };

			auto const add_resource = [&](glm::uvec4 const& source_region, std::string&& text)
			{
				auto icon = create_sprite();
				icon->set_render_texture(spritesheet);
				icon->set_position(resource_icon_offset += resource_icon_margin);
				icon->set_source_region(source_region);

				auto label = create_label();
				label->set_position(resource_icon_offset + glm::uvec2{ 16, 32 });
				label->set_font(smallFont);
				label->set_text(std::move(text));
				label->center_text();

				_loop->append(icon);
				_loop->append(label);
			};
			
			add_resource({ 32, 32, 32, 32 }, "energy");
			add_resource({ 64, 32, 32, 32 }, "water");
			add_resource({ 96, 32, 32, 32 }, "food");
			add_resource({ 128, 32, 32, 32 }, "workforce");

			_loop->append(stage);
			_loop->append(_tile_info_sprite);
			_loop->append(_tile_info_coordinate_label);
			_loop->append(_tile_info_terrain_label);
			_loop->append(create_fps_counter(defaultFont));
		}

		void run_and_wait_internal() override
		{
			auto display_to_backbuffer_factor = static_cast<glm::vec2>(_display->backbuffer_size()) / static_cast<glm::vec2>(_display->display_size());

			_drawable_grid->set_position(_display->backbuffer_size() / glm::uvec2{ 2, 2 });

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

				// todo: less stupid key handling required
				if (_keyboard->is_key_down(ALLEGRO_KEY_ESCAPE))
				{
					_continueRendering.store(false);
				}
			}
		}

		void update_highlighted_tile(std::optional<glm::uvec2 const> const& tile_coordinate)
		{
			if (!_tile_info_coordinate_label)
			{
				return;
			}


			if (tile_coordinate)
			{
				_tile_info_sprite->set_visible(true);
				_tile_info_terrain_label->set_visible(true);
				_tile_info_coordinate_label->set_visible(true);

				auto tile_info = _grid->lookup(*tile_coordinate);
				std::stringstream buffer{};
				buffer << (*tile_coordinate).x << ", " << (*tile_coordinate).y;

				_tile_info_sprite->set_source_region({ 0, 0, 32, 32 });
				_tile_info_terrain_label->set_text(std::string{ tile_info->terrain().name });
				_tile_info_coordinate_label->set_text(buffer.str());
			}
			else
			{
				_tile_info_terrain_label->set_visible(false);
				_tile_info_coordinate_label->set_visible(false);
				_tile_info_sprite->set_visible(false);
			}

		}

		mutable std::atomic<bool> _continueRendering { true };
		std::shared_ptr<environment> const _environment{ std::move(create_environment()) };
		std::shared_ptr<display> _display{ nullptr };
		std::shared_ptr<display_loop> _loop{ nullptr };
		std::shared_ptr<mouse> _mouse{ nullptr };
		std::shared_ptr<keyboard> _keyboard{ nullptr };

		std::shared_ptr<sprite> _tile_info_sprite{ nullptr };
		std::shared_ptr<label> _tile_info_coordinate_label { nullptr };
		std::shared_ptr<label> _tile_info_terrain_label{ nullptr };

		std::shared_ptr<grid> _drawable_grid{ nullptr };
		std::shared_ptr<rectangular_grid> _grid { create_rectangular_grid({ 20, 20 }) };
	};
}

namespace engine
{
	std::shared_ptr<app> create_app()
	{
		return std::move(std::make_shared<app_internal>());
	}
}