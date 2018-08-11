#pragma once

#include <list>

#include <glm.hpp>

#include "details/aloo_empty.h"

namespace engine
{
	namespace aloo
	{
		class draw_context;

		class drawable
		{
		public:
			virtual ~drawable() {}

			inline void draw(draw_context& context)
			{
				draw_internal(context);
			}

			inline bool draw_requested(draw_context& context) const
			{
				return draw_requested_internal(context);
			}

			inline std::weak_ptr<drawable>& parent()
			{
				return parent_internal();
			}

			inline std::list<std::shared_ptr<drawable>> const& children() const
			{
				return children_internal();
			}

			inline void append(std::shared_ptr<drawable> const& drawable)
			{
				append_internal(drawable);
			}

			inline glm::ivec2 position() const
			{
				return position_internal();
			}

			inline void set_position(glm::ivec2 const& position)
			{
				set_position_internal(position);
			}

			inline void set_tint(glm::lowp_vec4 const& opacity)
			{
				set_tint_internal(opacity);
			}

			inline void set_visible(bool const& value)
			{
				set_visible_internal(value);
			}
		private:
			virtual void draw_internal(draw_context& context) {};
			virtual bool draw_requested_internal(draw_context& context) const { return false; };
			virtual std::weak_ptr<drawable>& parent_internal() { static std::weak_ptr<drawable> v{}; return v; };
			virtual std::list<std::shared_ptr<drawable>> const& children_internal() const { return details::empty<std::list<std::shared_ptr<drawable>>>(); };
			virtual void append_internal(std::shared_ptr<drawable> const& drawable) { };
			virtual glm::ivec2 position_internal() const { return {}; }
			virtual void set_position_internal(glm::ivec2 const& position) { };
			virtual void set_tint_internal(glm::lowp_vec4 const& opacity) { };
			virtual void set_visible_internal(bool const& value) { };
		};
	}
}