#include "pch_allegro.h"
#pragma hdrstop

#include <filesystem>

#include "alasthope/path_utils.h"

#include "utils.h"
#include "render_texture_lock_op.h"
#include "render_target_op.h"

#include "details/bitmap_flags_guard.h"
#include "details/bitmap_format_guard.h"

#include "render_texture.h"

namespace
{
	using namespace engine::aloo;

	class render_texture_internal : public render_texture, public std::enable_shared_from_this<render_texture_internal>
	{
	public:
		render_texture_internal(ALLEGRO_BITMAP* allegro_texture_ptr)
			: _format{ al_get_bitmap_format(allegro_texture_ptr) }
		{
			_texture = std::move(al_bitmap{ allegro_texture_ptr });
		}

		render_texture_internal(size_t const& width, size_t const& height, render_texture_flags const& flags, render_texture_format const& format)
			: _format{ format }
		{
			{
				details::allegro_bitmap_flags_guard flagsGuard { static_cast<int>(flags) };
				details::allegro_bitmap_format_guard formatGuard { static_cast<int>(format) };

				_texture = al_bitmap{ al_create_bitmap(static_cast<int>(width), static_cast<int>(height)) };
				if (!_texture)
				{
					throw std::exception{ "unable to create no texture." };
				}
			}
		}

		inline ALLEGRO_BITMAP* get_native_ptr() const
		{
			return _texture.get();
		}

		inline render_texture_format format() const
		{
			return _format;
		}

	private:
		void* get_native_ptr_internal() const override
		{
			return _texture.get();
		}

		std::shared_ptr<render_texture> make_hardware_texture_internal() const override
		{
			auto allegro_texture_ptr = al_clone_bitmap(_texture.get());
			return std::move(std::make_shared<render_texture_internal>(allegro_texture_ptr));
		}

		std::shared_ptr<render_texture_lock_op> lock_internal() override
		{
			return std::move(create_lock_internal<>());
		}

		render_target_op make_render_target_internal() const override
		{
			return std::move(render_target_op{ _texture.get() });
		}

		template <bool placeholder = false>
		std::shared_ptr<render_texture_lock_op> create_lock_internal()
		{
			return std::make_shared<render_texture_lock_op_internal>(shared_from_this());
		}

		render_texture_format const _format;
		al_bitmap _texture{ nullptr };
	};

	class render_texture_lock_op_internal : public render_texture_lock_op
	{
	public:
		~render_texture_lock_op_internal()
		{
			al_unlock_bitmap(_texture->get_native_ptr());
		}

		render_texture_lock_op_internal(std::shared_ptr<render_texture_internal> const& render_texture)
			: _texture{ render_texture }
		{
			_locked_region = al_lock_bitmap(render_texture->get_native_ptr(), static_cast<int>(render_texture->format()), ALLEGRO_LOCK_WRITEONLY);
		}
	private:
		void* get_data_ptr_internal() const override
		{
			return _locked_region->data;
		}

		ALLEGRO_LOCKED_REGION* _locked_region{  nullptr };
		std::shared_ptr<render_texture_internal> const _texture;
	};
}

namespace engine
{
	namespace aloo
	{
		std::shared_ptr<render_texture> create_render_texture(std::filesystem::path const& path)
		{
			auto localPath = make_local_path(path);

			if (localPath.empty())
			{
				throw std::exception{ "file is outside of the application directory." };
			}

			al_bitmap bitmap{ al_load_bitmap(localPath.u8string().c_str()) };
			auto result = std::move(std::make_shared<render_texture_internal>(bitmap.get()));
			bitmap.release();
			return std::move(result);
		}

		std::shared_ptr<render_texture> create_render_texture(size_t const& width, size_t const& height, render_texture_flags const& flags, render_texture_format const& format)
		{
			return std::move(std::make_shared<render_texture_internal>(width, height, flags, format));
		}
	}
}