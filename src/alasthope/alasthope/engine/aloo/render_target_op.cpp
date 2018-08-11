#include "pch_allegro.h"
#pragma hdrstop

#include "render_target_op.h"

namespace engine
{
	namespace aloo
	{
		render_target_op::~render_target_op()
		{
			al_set_target_backbuffer(al_get_current_display());
		}

		render_target_op::render_target_op(void* const native_texture_ptr)
		{
			al_set_target_bitmap(reinterpret_cast<ALLEGRO_BITMAP*>(native_texture_ptr));
		}
	}
}