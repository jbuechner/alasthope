#include "pch_allegro.h"
#pragma hdrstop

#include "alasthope/engine/aloo/render_texture_flag.h"

namespace
{
	using namespace engine::aloo;

	static void __assert_render_texture_flags()
	{
		using t = std::underlying_type_t<render_texture_flags>;
		static_assert(static_cast<t>(render_texture_flags::memory) == ALLEGRO_MEMORY_BITMAP, "");
		static_assert(static_cast<t>(render_texture_flags::min_linear) == ALLEGRO_MIN_LINEAR, "");
		static_assert(static_cast<t>(render_texture_flags::mag_linear) == ALLEGRO_MAG_LINEAR, "");
		static_assert(static_cast<t>(render_texture_flags::video) == ALLEGRO_VIDEO_BITMAP, "");
		static_assert(static_cast<t>(render_texture_flags::convert) == ALLEGRO_CONVERT_BITMAP, "");
		static_assert(static_cast<t>(render_texture_flags::no_preserve) == ALLEGRO_NO_PRESERVE_TEXTURE, "");
		static_assert(static_cast<t>(render_texture_flags::alpha_test) == ALLEGRO_ALPHA_TEST, "");
		static_assert(static_cast<t>(render_texture_flags::mipmap) == ALLEGRO_MIPMAP, "");
	}
}