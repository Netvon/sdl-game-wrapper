#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "errors.h"

namespace sdl {
	struct lib_ttf {
		lib_ttf(const lib_ttf&) = delete;
		lib_ttf(lib_ttf&&) = delete;
		lib_ttf& operator=(const lib_ttf&) = delete;
		lib_ttf& operator=(lib_ttf&&) = delete;

		lib_ttf() {
			if (TTF_Init() != 0) {
				throw init_error();
			}
		}

		~lib_ttf() {
			TTF_Quit();
		}
	};
}