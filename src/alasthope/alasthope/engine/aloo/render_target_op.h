#pragma once

namespace engine
{
	namespace aloo
	{
		class render_target_op
		{
		public:
			~render_target_op();
			render_target_op(render_target_op&& other);
			render_target_op(void* const native_texture_ptr);
		private:
			bool _owning{ true };
		};
	}
}