#pragma once

#include <memory>
#include <filesystem>

namespace engine
{
	namespace aloo
	{
		class font;

		class font_manager
		{
		public:
			virtual ~font_manager() {};

			std::shared_ptr<font> const get_font(std::filesystem::path const& path, uint16_t const& sizeInPixel)
			{
				return get_font_internal(path, sizeInPixel);
			}

		private:
			virtual std::shared_ptr<font> const get_font_internal(std::filesystem::path const& path, uint16_t const& sizeInPixel) = 0;
		};
	}
}