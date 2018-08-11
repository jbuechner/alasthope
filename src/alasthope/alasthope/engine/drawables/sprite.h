#pragma once

#include <memory>

#include "alasthope/engine/aloo/drawable.h"

namespace engine
{
	namespace aloo
	{
		class render_texture;
	}

	namespace drawables
	{
		class sprite : public engine::aloo::drawable
		{
		public:
			void set_render_texture(std::shared_ptr<aloo::render_texture> const& texture)
			{
				set_render_texture_internal(texture);
			}

			void set_source_region(glm::vec4 const& region)
			{
				set_source_region_internal(region);
			}
		private:
			virtual void set_render_texture_internal(std::shared_ptr<aloo::render_texture> const& texture) = 0;
			virtual void set_source_region_internal(glm::vec4 const& region) = 0;
		};

		std::shared_ptr<sprite> create_sprite();
	}
}