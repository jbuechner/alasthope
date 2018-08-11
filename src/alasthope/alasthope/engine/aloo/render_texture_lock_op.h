#pragma once

namespace engine
{
	namespace aloo
	{
		class render_texture_lock_op
		{
		public:
			inline void* get_data_ptr() const
			{
				return get_data_ptr_internal();
			}
		protected:
			virtual ~render_texture_lock_op() {}
		private:
			virtual void* get_data_ptr_internal() const = 0;
		};
	}
}