#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string_view>
#include "errors.h"
#include "surface.h"

namespace sdl {
	struct lib_image {
		lib_image() = delete;
		lib_image(const lib_image&) = delete;
		lib_image(lib_image&&) = delete;
		lib_image& operator=(const lib_image&) = delete;
		lib_image& operator=(lib_image&&) = delete;

		explicit lib_image(Uint32 flags) {
			if (IMG_Init(flags) == 0) {
				throw init_error();
			}
		}

		//sdl::surface load_image(std::string_view path) const {
		//	return sdl::surface(IMG_Load(path.data()));
		//}

		~lib_image() {
			IMG_Quit();
		}
	};
}