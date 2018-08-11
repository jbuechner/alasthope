#pragma once

#include <allegro5/allegro.h>

namespace engine
{
	namespace aloo
	{
		namespace details
		{
			class allegro_bitmap_flags_guard
			{
			public:
				~allegro_bitmap_flags_guard()
				{
					al_set_new_bitmap_flags(_previous_flags);
				}

				allegro_bitmap_flags_guard(int flags)
					: _previous_flags{ al_get_new_bitmap_flags() }
				{
					al_set_new_bitmap_flags(flags);
				}

			private:
				int const _previous_flags;
			};
		}
	}
}