#pragma once

#include "pch.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#ifdef _DEBUG
#pragma comment(lib, "allegro-debug.lib")
#pragma comment(lib, "allegro_color-debug.lib")
#pragma comment(lib, "allegro_image-debug.lib")
#pragma comment(lib, "allegro_primitives-debug.lib")
#pragma comment(lib, "allegro_font-debug.lib")
#pragma comment(lib, "allegro_ttf-debug.lib")
#else
#pragma comment(lib, "allegro.lib")
#pragma comment(lib, "allegro_color.lib")
#pragma comment(lib, "allegro_image.lib")
#pragma comment(lib, "allegro_primitives.lib")
#pragma comment(lib, "allegro_font.lib")
#pragma comment(lib, "allegro_ttf.lib")
#endif