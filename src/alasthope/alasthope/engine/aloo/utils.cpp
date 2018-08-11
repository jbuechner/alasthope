#include "pch_allegro.h"
#pragma hdrstop

#include "countlut.h"

namespace engine
{
	namespace aloo
	{
		void to_string(int32_t const& value, char* const buffer)
		{
			u64toa_countlut(value, buffer);
		}
	}
}