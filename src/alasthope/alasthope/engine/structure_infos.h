#pragma once

#include "structure_info.h"

namespace engine
{
	structure_info const& lookup_structure_none();
	structure_info const& lookup_structure_info(size_t const id);
}