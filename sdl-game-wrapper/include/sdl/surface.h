#pragma once
#include <SDL.h>
#include "errors.h"

namespace sgw {
	struct image_manager;
}

namespace sdl {
	struct renderer;
	struct font;
	struct texture;

	struct surface {
		surface() = default;
		surface(const surface&) = delete;
		surface(surface&& other) noexcept {
			std::swap(m_surface_ptr, other.m_surface_ptr);
		};

		surface& operator=(const surface&) = delete;
		surface& operator=(surface&& other) noexcept {
			std::swap(m_surface_ptr, other.m_surface_ptr);
			return *this;
		}

		~surface() {
			if (m_surface_ptr != nullptr) {
				SDL_FreeSurface(m_surface_ptr);
			}
		}

		[[nodiscard]] int get_width() const {
			if (m_surface_ptr == nullptr) {
				throw surface_null_error();
			}

			return m_surface_ptr->w;
		}

		[[nodiscard]] int get_height() const {
			if (m_surface_ptr == nullptr) {
				throw surface_null_error();
			}

			return m_surface_ptr->h;
		}

		[[nodiscard]] SDL_BlendMode get_blend_mode() const {
			SDL_BlendMode bm;
			if (SDL_GetSurfaceBlendMode(m_surface_ptr, &bm) == -1) {
				// TODO: throw error;
			}
			return bm;
		}

		void set_blend_mode(SDL_BlendMode blend_mode) const {
			if (SDL_SetSurfaceBlendMode(m_surface_ptr, blend_mode) == -1) {
				// TODO: throw error;
			}
		}

	private:
		explicit surface(SDL_Surface* surface) : m_surface_ptr(surface) {
			if (m_surface_ptr == nullptr) {
				throw invalid_surface_error();
			}
		}

		SDL_Surface* m_surface_ptr = nullptr;
		friend renderer;
		friend font;
		friend texture;
		friend sgw::image_manager;
	};
}