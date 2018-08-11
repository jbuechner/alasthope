#pragma once

namespace engine
{
	namespace aloo
	{
		enum class render_texture_flags
		{
			memory = 1,
			no_preserve = 8,
			alpha_test = 16,
			min_linear = 64,
			mag_linear = 128,
			mipmap = 256,
			video = 1024,
			convert = 4096
		};
	}
}