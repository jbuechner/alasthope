#include "pch_allegro.h"
#pragma hdrstop

#include "alasthope/os/os.h"
#include "alasthope/engine/aloo/utils.h"
#include "alasthope/engine/aloo/font.h"

#include "alasthope/engine/aloo/parental.h"

#include "label.h"

namespace
{
	using namespace engine::aloo;
	using namespace engine::drawables;

	class label_internal : public label, std::enable_shared_from_this<label_internal>
	{
	private:
		virtual bool draw_requested_internal(draw_context& context) const override
		{
			return _is_visible;
		}

		virtual void draw_internal(draw_context& context) override
		{
			if (_font)
			{
				al_draw_text(reinterpret_cast<ALLEGRO_FONT*>(_font->get_native_ptr()), _color, _position.x, _position.y, _flags, _text.c_str());
			}
			_is_dirty = false;
		}

		virtual std::weak_ptr<drawable>& parent_internal() override
		{
			return _parental.parent();
		}

		void set_visible_internal(bool const& value) override
		{
			_is_visible = value;
		}

		void set_text_internal(std::string&& text) override
		{
			_text = std::move(text);
		}

		void set_position_internal(glm::ivec2 const& position) override
		{
			_position = position;
			_is_dirty = true;
		}

		void set_font_internal(std::shared_ptr<font> const& font) override
		{
			_font = font;
			_is_dirty = true;
		}

		void center_text_internal() override
		{
			_flags |= ALLEGRO_ALIGN_CENTRE;
		}

		int _flags{ allegro_draw_text_flags() };
		ALLEGRO_COLOR _color{ al_map_rgb(0x22, 0x22, 0x22) };
		bool _is_dirty{ true };
		bool _is_visible{ true };
		std::string _text{};
		parental<label_internal> _parental{};
		std::shared_ptr<font> _font{ nullptr };
		glm::ivec2 _position{};
	};
}

namespace engine
{
	namespace drawables
	{
		std::shared_ptr<label> create_label()
		{
			return std::make_shared<label_internal>();
		}
	}
}