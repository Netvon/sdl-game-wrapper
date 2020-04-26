#pragma once
#include <SDL.h>
#include <optional>
#include "errors.h"
#include "../gl/context.h"

namespace sdl {
	struct window {

		constexpr static auto position_centered = 0x2FFF0000U;

		window() = delete;
		window(const window&) = delete;
		window(window&& other) noexcept {
			std::swap(m_window_ptr, other.m_window_ptr);
		}

		window& operator=(const window&) = delete;
		window& operator=(window&& other) noexcept {
			std::swap(m_window_ptr, other.m_window_ptr);
			return *this;
		}

		window(const char* title, int x, int y, int w, int h, Uint32 flags) {
			m_window_ptr = SDL_CreateWindow(title, x, y, w, h, flags);
			if (m_window_ptr == nullptr) {
				throw window_create_error();
			}
		}

		~window() {
			if (m_window_ptr != nullptr) {
				SDL_DestroyWindow(m_window_ptr);
			}
		}

		void set_title(std::string_view title) const {
			SDL_SetWindowTitle(m_window_ptr, title.data());
		}

		[[nodiscard]] std::string_view get_title() const {
			return SDL_GetWindowTitle(m_window_ptr);
		}

		template <typename SizeType = std::pair<int, int>>
		[[nodiscard]] SizeType get_window_position() const {
			int x;
			int y;
			SDL_GetWindowPosition(m_window_ptr, &x, &y);

			return { x, y };
		}

		void set_window_position(int x, int y) const {
			SDL_SetWindowPosition(m_window_ptr, x, y);
		}

		template<typename SizeType = std::pair<int, int>>
		[[nodiscard]] SizeType get_window_size() const {
			int w;
			int h;
			SDL_GetWindowSize(m_window_ptr, &w, &h);

			return { w, h };
		}

		void set_window_size(int w, int h) const {
			SDL_SetWindowSize(m_window_ptr, w, h);
		}

		[[nodiscard]] int get_display_index() const {
			int val = SDL_GetWindowDisplayIndex(m_window_ptr);

			if (val < 0) {
				throw error();
			}

			return val;
		}

		[[nodiscard]] bool get_window_grab() const {
			return SDL_GetWindowGrab(m_window_ptr) == SDL_TRUE;
		}

		[[nodiscard]] Uint32 get_id() const {
			auto val = SDL_GetWindowID(m_window_ptr);

			if (val == 0) {
				throw error();
			}

			return val;
		}

		static window from_id(Uint32 window_id) {
			return window(window_id);
		}

	private:
		explicit window(SDL_Window* ptr) : m_window_ptr(ptr){};
		explicit window(Uint32 window_id) {
			auto m_window_ptr = SDL_GetWindowFromID(window_id);

			if (m_window_ptr == nullptr) {
				throw unknown_window_error();
			}
		}

		SDL_Window* m_window_ptr = nullptr;

		friend sgw::gl::context;
	};
}