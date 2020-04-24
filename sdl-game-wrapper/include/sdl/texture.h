#pragma once
#include <SDL.h>
#include "errors.h"
#include "surface.h"

namespace sdl {
	struct renderer;
	struct texture;

	struct guard_texture_color_mod {

		guard_texture_color_mod() = delete;
		guard_texture_color_mod(const guard_texture_color_mod&) = delete;
		guard_texture_color_mod(guard_texture_color_mod&& other) noexcept {
			std::swap(m_texture_ptr, other.m_texture_ptr);
			std::swap(m_original_color, other.m_original_color);
		}

		guard_texture_color_mod& operator=(const guard_texture_color_mod&) = delete;
		guard_texture_color_mod& operator=(guard_texture_color_mod&& other) noexcept {
			std::swap(m_texture_ptr, other.m_texture_ptr);
			return *this;
		}

		~guard_texture_color_mod() {
			if (m_texture_ptr != nullptr) {
				SDL_SetTextureColorMod(m_texture_ptr, m_original_color.r, m_original_color.g, m_original_color.b);
			}
		}

	private:

		explicit guard_texture_color_mod(SDL_Texture* p_texture_ptr) : m_texture_ptr{ p_texture_ptr } {
			if (m_texture_ptr != nullptr) {
				SDL_GetTextureColorMod(m_texture_ptr, &m_original_color.r, &m_original_color.g, &m_original_color.b);
			}
		}

		SDL_Texture* m_texture_ptr{ nullptr };
		SDL_Color m_original_color{ 0, 0, 0, 0 };

		friend texture;
	};

	
	struct guard_texture_alpha_mod {

		guard_texture_alpha_mod() = delete;
		guard_texture_alpha_mod(const guard_texture_alpha_mod&) = delete;
		guard_texture_alpha_mod(guard_texture_alpha_mod&& other) noexcept {
			std::swap(m_texture_ptr, other.m_texture_ptr);
			std::swap(m_original_alpha, other.m_original_alpha);
		}

		guard_texture_alpha_mod& operator=(const guard_texture_alpha_mod&) = delete;
		guard_texture_alpha_mod& operator=(guard_texture_alpha_mod&& other) noexcept {
			std::swap(m_texture_ptr, other.m_texture_ptr);
			return *this;
		}

		~guard_texture_alpha_mod() {
			if (m_texture_ptr != nullptr) {
				SDL_SetTextureAlphaMod(m_texture_ptr, m_original_alpha);
			}
		}

	private:
		explicit guard_texture_alpha_mod(SDL_Texture* p_texture_ptr) : m_texture_ptr{ p_texture_ptr } {
			if (m_texture_ptr != nullptr) {
				SDL_GetTextureAlphaMod(m_texture_ptr, &m_original_alpha);
			}
		}

		SDL_Texture* m_texture_ptr{ nullptr };
		uint8_t m_original_alpha{ 0 };

		friend texture;
	};


	struct texture {
		texture() = default;
		texture(const texture&) = delete;
		texture(texture&& other) noexcept {
			std::swap(m_texture_ptr, other.m_texture_ptr);
		};

		texture& operator=(const texture&) = delete;
		texture& operator=(texture&& other) noexcept {
			std::swap(m_texture_ptr, other.m_texture_ptr);
			return *this;
		}

		//template<typename Renderer>
		//texture(const sdl::surface& surface, const Renderer& renderer) {
		//	m_texture_ptr = SDL_CreateTextureFromSurface(renderer.m_renderer_ptr, surface.m_surface_ptr);

		//	if (m_texture_ptr == nullptr) {
		//		throw invalid_texture_error();
		//	}
		//}

		[[nodiscard]] SDL_BlendMode get_blend_mode() const {
			SDL_BlendMode bm;
			if (SDL_GetTextureBlendMode(m_texture_ptr, &bm) == -1) {
				// TODO: throw error;
			}
			return bm;
		}

		void set_blend_mode(SDL_BlendMode blend_mode) const {
			if (SDL_SetTextureBlendMode(m_texture_ptr, blend_mode) == -1) {
				// TODO: throw error;
			}
		}

		void set_alpha_mod(uint8_t alpha) const {
			if (SDL_SetTextureAlphaMod(m_texture_ptr, alpha) == -1) {
				// TODO: throw error;
			}
		}

		[[nodiscard]] uint8_t get_alpha_mod() const {
			uint8_t alpha;
			if (SDL_GetTextureAlphaMod(m_texture_ptr, &alpha) == -1) {
				// TODO: throw error;
			}

			return alpha;
		}

		template <typename ColorValue = SDL_Color>
		void set_color_mod(const ColorValue& color) const {
			set_color_mod(color.r, color.g, color.b);
		}

		void set_color_mod(uint8_t r, uint8_t g, uint8_t b) const {
			if (SDL_SetTextureColorMod(m_texture_ptr, r, g, b) == -1) {
				// TODO: throw error;
			}
		}

		template <typename ColorValue = SDL_Color>
		[[nodiscard]] ColorValue get_color_mod() const {
			uint8_t r, g, b;

			if (SDL_GetTextureColorMod(m_texture_ptr, &r, &g, &b) == -1) {
				// TODO: throw error;
			}

			return { r, g, b };
		}

		[[nodiscard]] guard_texture_color_mod get_color_mod_guard() const {
			return guard_texture_color_mod(m_texture_ptr);
		}

		[[nodiscard]] guard_texture_alpha_mod get_alpha_mod_guard() const {
			return guard_texture_alpha_mod(m_texture_ptr);
		}

		~texture() {
			if (m_texture_ptr != nullptr) {
				SDL_DestroyTexture(m_texture_ptr);
			}
		}

		template<typename SizeType = std::pair<int, int>>
		SizeType get_size() const {
			int w, h;
			if (SDL_QueryTexture(m_texture_ptr, nullptr, nullptr, &w, &h) == -1) {
				throw invalid_texture_error();
			}

			return { w, h };
		}

	private:
		explicit texture(SDL_Texture* texture) : m_texture_ptr(texture) {
			if (m_texture_ptr == nullptr) {
				throw invalid_texture_error();
			}
		}

		SDL_Texture* m_texture_ptr = nullptr;
		friend renderer;
	};
}