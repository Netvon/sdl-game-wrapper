#pragma once
#include "../sdl/errors.h"
#include <glad.h>

namespace sdl {
	struct window;
}

namespace sgw::gl {

	struct context {

		context() = default;
		context(const context&) = delete;
		context(context&& other) noexcept {
			std::swap(m_gl_context, other.m_gl_context);
		}

		context& operator=(const context&) = delete;
		context& operator=(context&& other) noexcept {
			std::swap(m_gl_context, other.m_gl_context);
			return *this;
		}

		explicit context(const sdl::window& window);

		~context() {
			if (m_gl_context != nullptr) {
				SDL_GL_DeleteContext(m_gl_context);
			}
		}

		void make_current(const sdl::window& window) const;
		static void set_enable(unsigned int value);

	private:
		SDL_GLContext m_gl_context = nullptr;
	};
}