#ifndef SGW_GL_PROGRAM_H
#define SGW_GL_PROGRAM_H
#include "shader.h"

#include <array>
#include <cstddef>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace sgw {

    struct program_link_error : public std::runtime_error {
        explicit program_link_error(const std::string& error)
            : std::runtime_error(error) {}
    };

    struct unknown_uniform_error : public std::runtime_error {
        explicit unknown_uniform_error(const std::string& name)
            : std::runtime_error(name) {}
    };

    struct program {
        using id = GLint;

        constexpr static std::size_t default_info_log_size = 4096;

        program() = default;

        explicit program(id program)
            : m_program(program) {}

        program(const program&) noexcept = default;
        program(program&&) noexcept      = default;
        program& operator=(program&&) noexcept = default;
        program& operator=(const program&) noexcept = default;
        ~program()                                  = default;

        void attach_shader(const shader& shader) const { glAttachShader(m_program, shader.get_id()); }

        template <typename... Shaders>
        void attach_shaders(const Shaders&... shader) const {
            (attach_shader(shader), ...);
        }

        void link() const {
            glLinkProgram(m_program);

            if (!get_link_status()) { throw program_link_error(get_info_log()); }
        }

        [[nodiscard]] bool get_link_status() const {
            GLint params = -1;
            glGetProgramiv(m_program, GL_LINK_STATUS, &params);

            return GL_TRUE == params;
        }

        static void link(id program) { glLinkProgram(program); }

        void use() const { glUseProgram(m_program); }

        static void use(id program) { glUseProgram(program); }

        [[nodiscard]] std::string get_info_log() const {
            int actual_length = 0;
            std::array<char, default_info_log_size> log{};
            glGetProgramInfoLog(m_program, log.size(), &actual_length, log.data());

            return std::string(log.data());
        }

        void remove() const { glDeleteProgram(m_program); }

        [[nodiscard]] bool get_is_valid() const { return glIsProgram(m_program) == GL_TRUE; }

        [[nodiscard]] static program create() {
            program temp;
            temp.m_program = glCreateProgram();

            return temp;
        }

        template <typename... Shaders>
        [[nodiscard]] static program create_and_link(const Shaders&... shader) {
            program temp;
            temp.m_program = glCreateProgram();

            (temp.attach_shader(shader), ...);

            temp.link();

            (shader.remove(), ...);

            return temp;
        }

        [[nodiscard]] id get_id() const noexcept { return m_program; }

        [[nodiscard]] int get_uniform_location(const char* name) const { return glGetUniformLocation(m_program, name); }

        [[nodiscard]] unsigned int get_resource_location(const char* name, unsigned int program_interface) {
            return glGetProgramResourceIndex(m_program, program_interface, name);
        }

        [[nodiscard]] unsigned int get_active_resource_count(GLenum interface) const {
            GLint resource_count = 0;
            glGetProgramInterfaceiv(get_id(), interface, GL_ACTIVE_RESOURCES, &resource_count);

            return resource_count;
        }

        template <typename Type>
        void set(const char* name, const Type& value) const {
            auto location = get_uniform_location(name);

            if (location == -1) { throw unknown_uniform_error(name); }

            if constexpr (std::is_same_v<Type, float>) { glUniform1f(location, value); }
            else if constexpr (std::is_same_v<Type, int> || std::is_same_v<Type, bool>) {
                glUniform1i(location, static_cast<int>(value));
            }
            else if constexpr (std::is_same_v<Type, double>) {
                glUniform1d(location, value);
            }
            else if constexpr (std::is_same_v<Type, unsigned int>) {
                glUniform1ui(location, value);
            }
            else if constexpr (std::is_same_v<Type, glm::vec1>) {
                glUniform1fv(location, 1, glm::value_ptr(value));
            }
            else if constexpr (std::is_same_v<Type, glm::vec2>) {
                glUniform2fv(location, 1, glm::value_ptr(value));
            }
            else if constexpr (std::is_same_v<Type, glm::vec3>) {
                glUniform3fv(location, 1, glm::value_ptr(value));
            }
            else if constexpr (std::is_same_v<Type, glm::vec4>) {
                glUniform4fv(location, 1, glm::value_ptr(value));
            }
            else if constexpr (std::is_same_v<Type, glm::mat2>) {
                glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
            }
            else if constexpr (std::is_same_v<Type, glm::mat3>) {
                glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
            }
            else if constexpr (std::is_same_v<Type, glm::mat4>) {
                glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
            }
            else {
                static_assert(false, "SGW-GL: Set operation on program not supported for this type.");
            }
        }

        template <typename Type>
        void set(const std::string& name, const Type& value) const {
            set(name.c_str(), value);
        }

    private:
        id m_program{};
    };
}
#endif // SGW_GL_PROGRAM_H