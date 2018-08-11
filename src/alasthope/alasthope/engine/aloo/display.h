#pragma once

#include <memory>

namespace engine
{
	namespace aloo
	{
		class environment;
		class display_loop;
		class render_texture;
		struct display_options;

		class display
		{
		public:
			inline std::shared_ptr<display_loop> create_loop()
			{
				return create_loop_internal();
			}

			inline void get_size(glm::uvec2& value) const
			{
				get_size_internal(value);
			}

			inline render_texture& get_backbuffer() const
			{
				return get_backbuffer_internal();
			}

			inline bool backbuffer_and_frontbuffer_are_perfect() const
			{
				return backbuffer_and_frontbuffer_are_perfect_internal();
			}

			inline glm::uvec2 const& display_size() const
			{
				return display_size_internal();
			}

			inline glm::uvec2 const& backbuffer_size() const
			{
				return backbuffer_size_internal();
			}
		protected:
			virtual ~display() {};

		private:
			virtual std::shared_ptr<display_loop> create_loop_internal() = 0;
			virtual void get_size_internal(glm::uvec2& value) const = 0;
			virtual render_texture& get_backbuffer_internal() const = 0;
			virtual bool backbuffer_and_frontbuffer_are_perfect_internal() const = 0;
			virtual glm::uvec2 const& display_size_internal() const = 0;
			virtual glm::uvec2 const& backbuffer_size_internal() const = 0;
		};

		std::shared_ptr<display> create_display(std::shared_ptr<environment> const& environment, display_options const& options);
	}
}
