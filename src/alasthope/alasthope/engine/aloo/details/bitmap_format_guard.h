#pragma once

#include <allegro5/allegro.h>

namespace engine
{
	namespace aloo
	{
		namespace details
		{
			class allegro_bitmap_format_guard
			{
			public:
				~allegro_bitmap_format_guard()
				{
					al_set_new_bitmap_format(_previous_format);
				}

				allegro_bitmap_format_guard(int flags)
					: _previous_format{ al_get_new_bitmap_format() }
				{
					al_set_new_bitmap_format(flags);
				}

			private:
				int const _previous_format;
			};
		}
	}
}