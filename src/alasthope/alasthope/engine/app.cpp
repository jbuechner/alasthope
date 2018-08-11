#include "pch_allegro.h"
#pragma hdrstop

#include <atomic>
#include <thread>
#include <chrono>
#include <filesystem>

#include "alasthope/engine/aloo/environment.h"
#include "alasthope/engine/aloo/font_manager.h"
#include "alasthope/engine/aloo/keyboard.h"
#include "alasthope/engine/aloo/display.h"
#include "alasthope/engine/aloo/display_loop.h"
#include "alasthope/engine/aloo/render_texture.h"
#include "alasthope/engine/drawables/stage.h"
#include "alasthope/engine/drawables/fps_counter.h"
#include "alasthope/engine/drawables/label.h"
#include "alasthope/engine/drawables/sprite.h"

#include "app.h"

namespace
{
	using namespace engine::aloo;
	using namespace engine::drawables;

	class app_internal : public engine::app
	{
	public:
		app_internal()
		{
			_keyboard = std::move(create_keyboard(_environment));
			_display = std::move(create_display(_environment));
		}

	private:
		void run_and_wait_internal() const override
		{
			auto& display = *_display;
			auto loop = display.create_loop();

			auto const defaultFont = _environment->get_font_manager()->get_font(std::filesystem::path { "gfx" } / std::filesystem::path { "hack-regular.ttf" }, 14);

			auto stage = create_stage();
			loop->set_root_drawable(stage);
			stage->append(create_fps_counter(defaultFont));

			auto label{ create_label() };
			stage->append(label);
			label->set_font(defaultFont);

			auto spritesheet{ create_render_texture(std::filesystem::path{ "gfx" } / std::filesystem::path{"spritesheet.png"}) };
			auto sprite{ create_sprite() };
			sprite->set_render_texture(spritesheet);
			stage->append(sprite);
			sprite->set_position({ 100, 100 });
			sprite->set_source_region({ 16, 0, 32, 32 });
			sprite->set_tint({ 1, 0, 0, 0.5 });

			auto sprite2{ create_sprite() };
			sprite2->set_render_texture(spritesheet);
			stage->append(sprite2);
			sprite2->set_position({ 110, 105 });
			sprite2->set_source_region({ 16, 0, 32, 32 });

			label->set_text("Hallo");

			while ((*loop)() && _continueRendering.load())
			{
				_keyboard->process_events();
				// todo: less stupid key handling required
				if (_keyboard->is_key_down(ALLEGRO_KEY_ESCAPE))
				{
					_continueRendering.store(false);
				}
			}
		}

		mutable std::atomic<bool> _continueRendering { true };
		std::shared_ptr<environment> const _environment{ std::move(create_environment()) };
		std::shared_ptr<display> _display{ nullptr };
		std::shared_ptr<keyboard> _keyboard{ nullptr };
	};
}

namespace engine
{
	std::shared_ptr<app> create_app()
	{
		return std::move(std::make_shared<app_internal>());
	}
}