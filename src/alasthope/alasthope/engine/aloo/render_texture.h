#pragma once

#include <memory>
#include <filesystem>

#include "render_texture_format.h"
#include "render_texture_flag.h"
#include "render_target_op.h"

namespace engine
{
	namespace aloo
	{
		class render_texture_lock_op;
		class render_target_op;

		class render_texture
		{
		public:
			inline void* get_native_ptr() const
			{
				return get_native_ptr_internal();
			}

			inline std::shared_ptr<render_texture_lock_op> lock()
			{
				return lock_internal();
			}

			inline std::unique_ptr<render_target_op> make_render_target() const
			{
				return std::move(make_render_target_internal());
			}

			inline std::shared_ptr<render_texture> make_hardware_texture() const
			{
				return std::move(make_hardware_texture_internal());
			}
		protected:
			virtual ~render_texture() {};
		private:
			virtual void* get_native_ptr_internal() const = 0;
			virtual std::shared_ptr<render_texture_lock_op> lock_internal() = 0;
			virtual std::unique_ptr<render_target_op> make_render_target_internal() const = 0;
			virtual std::shared_ptr<render_texture> make_hardware_texture_internal() const = 0;
		};

		std::shared_ptr<render_texture> create_render_texture(std::filesystem::path const& path);
		std::shared_ptr<render_texture> create_render_texture(size_t const& width, size_t const& height, render_texture_flags const& flags, render_texture_format const& format);
	}
}