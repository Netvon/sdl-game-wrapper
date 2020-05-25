#ifndef SGW_GL_CONTEXT_H
#define SGW_GL_CONTEXT_H
#include "../sdl/errors.h"

#include <glad/glad.h>

namespace sdl {
    struct window;
}

namespace sgw {

    struct context {

        context()               = default;
        context(const context&) = delete;
        context(context&& other) noexcept { std::swap(m_gl_context, other.m_gl_context); }

        context& operator=(const context&) = delete;
        context& operator                  =(context&& other) noexcept {
            std::swap(m_gl_context, other.m_gl_context);
            return *this;
        }

        explicit context(const sdl::window& window);

        ~context() = default;

        void make_current(const sdl::window& window) const;
        static void set_enable(unsigned int value);

        [[nodiscard]] SDL_GLContext get_ptr() const noexcept { return m_gl_context; }

    private:
        SDL_GLContext m_gl_context = nullptr;
    };
}
#endif // SGW_GL_CONTEXT_H