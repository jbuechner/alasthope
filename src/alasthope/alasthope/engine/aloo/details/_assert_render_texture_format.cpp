#include "pch_allegro.h"
#pragma hdrstop

#include "alasthope/engine/aloo/render_texture_format.h"

namespace
{
	using namespace engine::aloo;

	static void __assert_render_texture_format()
	{
		using t = std::underlying_type_t<render_texture_format>;
		static_assert(static_cast<t>(render_texture_format::abgr_8888_le) == ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE, "");
	}
}