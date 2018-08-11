#include "pch_allegro.h"
#pragma hdrstop

#include "environment.h"

#include "details/font_manager_impl.h"

namespace
{
	using namespace engine::aloo;

	void void_fn()
	{
	}

	template <bool init()>
	class al_system
	{
	public:
		al_system(std::string const& initialization_error_msg)
		{
			if (!init())
			{
				throw std::exception{ initialization_error_msg.c_str() };
			}
		}
	};

	class environment_internal : public environment
	{
	public:
		~environment_internal()
		{
			al_shutdown_ttf_addon();
			al_shutdown_font_addon();
			al_shutdown_primitives_addon();
			al_shutdown_image_addon();

		}
		environment_internal()
		{
			bool success = al_init();
			_ASSERT(success);
			if (!success)
			{
				throw std::exception{ "unable to initialize allegro." };
			}

			al_system<&al_init_image_addon>{"unable to initialize image addon."};
			al_system<&al_init_primitives_addon>{ "unable to initialize primitives addon." };
			al_system<&al_init_font_addon>{ "unable to initialize font addon." };
			al_system<&al_init_ttf_addon>{ "unable to initialize ttf addon." };

			al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_32_WITH_ALPHA);
			al_set_new_bitmap_flags(ALLEGRO_NO_PREMULTIPLIED_ALPHA);

			//al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
			al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
		}
	private:
		std::shared_ptr<font_manager> const& get_font_manager_internal() const
		{
			return _font_manager;
		}

		std::shared_ptr<font_manager> _font_manager{ std::make_shared<details::font_manager_impl>() };
	};
}

namespace engine
{
	namespace aloo
	{
		std::shared_ptr<environment> create_environment()
		{
			return std::make_shared<environment_internal>();
		}
	}
}