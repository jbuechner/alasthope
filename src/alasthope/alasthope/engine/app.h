#pragma once

#include <memory>

namespace engine
{
	class app
	{
	public:
		inline void run_and_wait() const
		{
			run_and_wait_internal();
		}

	protected:
		virtual ~app() {};

		virtual void run_and_wait_internal() const = 0;
	};

	std::shared_ptr<app> create_app();
}