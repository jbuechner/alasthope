#pragma once

#include "terrain_info.h"

namespace engine
{
	terrain_info const& lookup_terrain_none();
	terrain_info const& lookup_terrain_info(size_t const id);
}