#include "pch_allegro.h"
#pragma hdrstop

#include <filesystem>

#include "utils.h"
#include "font.h"

namespace
{
	using namespace engine::aloo;

	class font_internal : public font
	{
	public:
		font_internal(std::filesystem::path const& path, uint16_t const& sizeInPixel)
		{
			_font = al_load_font(path.generic_string().c_str(), -static_cast<int32_t>(sizeInPixel), 0);
		}

	private:
		void* get_native_ptr_internal() const override
		{
			return _font.get();
		}

		al_font _font{ nullptr };
	};
}

namespace engine
{
	namespace aloo
	{
		std::shared_ptr<font> create_font(std::filesystem::path const& path, uint16_t const& sizeInPixel)
		{
			return std::make_shared<font_internal>(path, sizeInPixel);
		}
	}
}