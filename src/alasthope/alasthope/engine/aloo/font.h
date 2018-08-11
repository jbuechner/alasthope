#pragma once

#include <memory>
#include <filesystem>

namespace engine
{
	namespace aloo
	{
		class font
		{
		public:
			virtual ~font() {};

			inline void* get_native_ptr() const
			{
				return get_native_ptr_internal();
			}

		private:
			virtual void* get_native_ptr_internal() const = 0;
		};

		std::shared_ptr<font> create_font(std::filesystem::path const& path, uint16_t const& sizeInPixel);
	}
}