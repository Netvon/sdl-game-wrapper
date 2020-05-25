#ifndef SGW_GL_ERROR_H
#define SGW_GL_ERROR_H
#include <glad/glad.h>

namespace sgw::gl {

    enum struct error_flag {
        no_error                      = GL_NO_ERROR,
        invalid_enum                  = GL_INVALID_ENUM,
        invalid_value                 = GL_INVALID_VALUE,
        invalid_operation             = GL_INVALID_OPERATION,
        invalid_framebuffer_operation = GL_INVALID_FRAMEBUFFER_OPERATION,
        out_of_memory                 = GL_OUT_OF_MEMORY,
        stack_underflow               = GL_STACK_UNDERFLOW,
        stack_overflow                = GL_STACK_OVERFLOW
    };

    struct error {
        error() = default;
        explicit error(error_flag flags) noexcept
            : m_flag(flags) {}

        [[nodiscard]] error_flag get_flag() const noexcept { return m_flag; }
        [[nodiscard]] bool is_error() const noexcept { return m_flag != error_flag::no_error; }

        explicit operator bool() const noexcept { return is_error(); }

    private:
        error_flag m_flag = error_flag::no_error;
    };

    error get_error() noexcept { return error(static_cast<error_flag>(glGetError())); }
}
#endif // SGW_GL_ERROR_H