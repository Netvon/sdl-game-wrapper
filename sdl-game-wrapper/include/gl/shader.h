#ifndef SGW_GL_SHADER_H
#define SGW_GL_SHADER_H
#include <array>
#include <cstddef>
#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace sgw {
    struct shader_compile_error : public std::runtime_error {
        explicit shader_compile_error(const std::string& error)
            : std::runtime_error(error) {}
    };

    struct shader {
        using id = GLint;

        constexpr static std::size_t default_info_log_size = 4096;

        constexpr static GLenum vertex_type   = GL_VERTEX_SHADER;
        constexpr static GLenum fragment_type = GL_FRAGMENT_SHADER;
        constexpr static GLenum compute_type  = GL_COMPUTE_SHADER;

        shader() = default;
        explicit shader(id shader)
            : m_shader(shader) {}

        shader(const shader&) noexcept = default;
        shader(shader&&) noexcept      = default;
        shader& operator=(shader&&) noexcept = default;
        shader& operator=(const shader&) noexcept = default;
        ~shader()                                 = default;

        void remove() const { glDeleteShader(m_shader); }

        [[nodiscard]] static shader create(GLenum shader_type) {
            shader temp;
            temp.m_shader = glCreateShader(shader_type);

            return temp;
        }

        [[nodiscard]] static shader create_and_compile(GLenum shader_type, const char* source_str) {
            shader temp = create(shader_type);
            glShaderSource(temp.m_shader, 1, &source_str, nullptr);
            glCompileShader(temp.m_shader);

            if (!temp.get_compile_status()) { throw shader_compile_error(temp.get_info_log()); }

            return temp;
        }

        [[nodiscard]] static shader create_and_compile_from_file(GLenum shader_type, const std::string& source_path) {
            shader temp;
            std::stringstream source_stream;

            std::ifstream stream(source_path);
            if (stream.is_open()) {
                source_stream << stream.rdbuf();
                return create_and_compile(shader_type, source_stream.str().c_str());
            }

            return temp;
        }

        [[nodiscard]] std::string get_info_log() const {
            int actual_length = 0;
            std::array<char, default_info_log_size> log{};
            glGetShaderInfoLog(m_shader, log.size(), &actual_length, log.data());

            return std::string(log.data());
        }

        [[nodiscard]] bool get_compile_status() const {
            GLint params = -1;
            glGetShaderiv(m_shader, GL_COMPILE_STATUS, &params);

            return GL_TRUE == params;
        }

        [[nodiscard]] id get_id() const noexcept { return m_shader; }
        [[nodiscard]] bool get_id_valid() const noexcept { return glIsShader(m_shader); }

    private:
        id m_shader{};
    };
}
#endif // SGW_GL_SHADER_H