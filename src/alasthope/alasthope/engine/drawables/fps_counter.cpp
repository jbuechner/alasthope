#include "pch_allegro.h"
#pragma hdrstop

#include <memory>

#include "alasthope/engine/aloo/utils.h"
#include "alasthope/engine/aloo/draw_context.h"
#include "alasthope/engine/aloo/font.h"
#include "alasthope/os/os.h"
#include "alasthope/engine/aloo/parental.h"

#include "fps_counter.h"

namespace
{
	using namespace engine::aloo;
	using namespace engine::drawables;

	class fps_counter_internal : public fps_counter
	{
	public:
		fps_counter_internal(std::shared_ptr<font> const& font)
			: _font{ font }
		{
		}
	private:
		void draw_internal(draw_context& context) override
		{
			al_draw_text(reinterpret_cast<ALLEGRO_FONT*>(_font->get_native_ptr()), al_map_rgb(0x22, 0x22, 0x22), 5, 5, allegro_draw_text_flags(), &context.fps_as_string()[0]);
		}

		bool draw_requested_internal(draw_context& context) const override
		{
			return std::get<0>(context.fps_measure_result());
		}

		std::weak_ptr<drawable>& parent_internal() override
		{
			return _parental.parent();
		}

		parental<fps_counter_internal> _parental{};
		std::shared_ptr<font> const _font;
	};
}

namespace engine
{
	namespace drawables
	{
		std::shared_ptr<fps_counter> create_fps_counter(std::shared_ptr<font> const& font)
		{
			return std::make_shared<fps_counter_internal>(font);
		}
	}
}