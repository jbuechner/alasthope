#pragma once

#include <memory>

namespace engine
{
	class app
	{
	public:
		inline void run_and_wait()
		{
			run_and_wait_internal();
		}

	protected:
		virtual ~app() {};

		virtual void run_and_wait_internal() = 0;
	};

	std::shared_ptr<app> create_app();
}