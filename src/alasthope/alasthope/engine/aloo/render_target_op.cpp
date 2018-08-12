#include "pch_allegro.h"
#pragma hdrstop

#include "render_target_op.h"

namespace engine
{
	namespace aloo
	{
		render_target_op::~render_target_op()
		{
			if (_existing)
			{
				al_set_target_bitmap(reinterpret_cast<ALLEGRO_BITMAP*>(_existing));
			}
			else
			{
				al_set_target_backbuffer(al_get_current_display());
			}
		}

		render_target_op::render_target_op(void* const native_texture_ptr)
			: _existing{ al_get_target_bitmap() }
		{
			al_set_target_bitmap(reinterpret_cast<ALLEGRO_BITMAP*>(native_texture_ptr));
		}
	}
}