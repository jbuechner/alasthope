#include "pch.h"
#pragma hdrstop

#include "engine/app.h"

int main()
{
	auto const app{ std::move(engine::create_app()) };
	app->run_and_wait();

	return 0;
}