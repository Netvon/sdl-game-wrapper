#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string_view>
#include "errors.h"
#include "lib_ttf.h"
#include "surface.h"
#include "texture.h"

namespace sdl {
	struct renderer;

	struct font {
		font() = delete;
		font(const font&) = delete;
		font(font&& other) noexcept {
			std::swap(m_font_ptr, other.m_font_ptr);
			std::swap(m_point_size, other.m_point_size);
		};

		font& operator=(const font&) = delete;
		font& operator=(font&& other) noexcept {
			std::swap(m_font_ptr, other.m_font_ptr);
			std::swap(m_point_size, other.m_point_size);
			return *this;
		}

		font(std::string_view path, int point_size) : m_point_size(point_size) {
			m_font_ptr = TTF_OpenFont(path.data(), point_size);

			if (m_font_ptr == nullptr) {
				throw font_open_error();
			}
		}

		~font() {
			if (m_font_ptr != nullptr) {
				TTF_CloseFont(m_font_ptr);
			}
		}

		[[nodiscard]] std::pair<int, int> text_size(std::string_view text) const {
			int w;
			int h;

			TTF_SizeText(m_font_ptr, text.data(), &w, &h);
			return { w, h };
		}

		[[nodiscard]] sdl::surface render_solid(std::string_view text, const SDL_Color& color) const {
			return sdl::surface (TTF_RenderText_Solid(m_font_ptr, text.data(), color));
		}

		[[nodiscard]] sdl::surface render_shaded(std::string_view text, const SDL_Color& foreground_color, const SDL_Color& background_color) const {
			return sdl::surface(TTF_RenderText_Shaded(m_font_ptr, text.data(), foreground_color, background_color));
		}

		[[nodiscard]] sdl::surface render_blended(std::string_view text, const SDL_Color& color) const {
			return sdl::surface(TTF_RenderText_Blended(m_font_ptr, text.data(), color));
		}


	private:
		TTF_Font* m_font_ptr = nullptr;
		int m_point_size = 0;
		friend renderer;
	};
}