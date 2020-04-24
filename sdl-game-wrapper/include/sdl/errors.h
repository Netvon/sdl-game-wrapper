#pragma once
#include <SDL.h>
#include <stdexcept>
#include <string_view>

namespace sdl {
	struct error : public std::runtime_error {
		error() : std::runtime_error(SDL_GetError()) {}
	};

	struct init_error : public std::runtime_error {
		init_error() : std::runtime_error(SDL_GetError()) {}
	};

	struct unknown_window_error : public std::runtime_error {
		unknown_window_error() : std::runtime_error(SDL_GetError()) {}
	};

	struct window_create_error : public std::runtime_error {
		window_create_error() : std::runtime_error(SDL_GetError()) {}
	};

	struct window_opengl_create_error : public std::runtime_error {
		window_opengl_create_error() : std::runtime_error("Could not load OpenGL") {}
	};

	struct opengl_context_create_error : public std::runtime_error {
		opengl_context_create_error() : std::runtime_error("Could not create the OpenGL Context") {}
	};

	struct renderer_create_error : public std::runtime_error {
		renderer_create_error() : std::runtime_error(SDL_GetError()) {}
	};

	struct renderer_copy_error : public std::runtime_error {
		renderer_copy_error() : std::runtime_error(SDL_GetError()) {}
	};

	struct renderer_draw_error : public std::runtime_error {
		renderer_draw_error() : std::runtime_error(SDL_GetError()) {}
	};

	struct renderer_draw_color_error : public std::runtime_error {
		renderer_draw_color_error() : std::runtime_error(SDL_GetError()) {}
	};

	struct font_open_error : public std::runtime_error {
		font_open_error() : std::runtime_error(SDL_GetError()) {}
	};

	struct invalid_texture_error : public std::runtime_error {
		invalid_texture_error() : std::runtime_error(SDL_GetError()) {}
	};

	struct invalid_surface_error : public std::runtime_error {
		invalid_surface_error() : std::runtime_error(SDL_GetError()) {}
	};

	struct surface_null_error : public std::runtime_error {
		surface_null_error() : std::runtime_error("Surface is null") {}
	};
}