#pragma once

#include <memory>
#include <list>

#include "drawable.h"

namespace engine
{
	namespace aloo
	{
		template <typename T>
		class hierarchical
		{
		public:
			hierarchical(T& owner)
				: _owner{ owner }
			{
			}

			std::weak_ptr<drawable>& parent()
			{
				return _parent;
			}

			std::list<std::shared_ptr<drawable>> const& children() const
			{
				return _children;
			}

			void append(std::shared_ptr<drawable> const& drawable)
			{
				_children.push_back(drawable);
				drawable->parent() = _owner.shared_from_this();
			}
		private:
			T& _owner;
			std::weak_ptr<drawable> _parent{};
			// todo: either intrusive or own thread safe pointer impl
			std::list<std::shared_ptr<drawable>> _children{};
		};
	}
}