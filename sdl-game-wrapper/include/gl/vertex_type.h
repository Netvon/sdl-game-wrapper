#ifndef SGW_GL_VERTEX_TYPE_H
#define SGW_GL_VERTEX_TYPE_H

#include <cstddef>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace sgw {
    namespace detail {
        template <typename Type>
        struct vertex_type {
            static constexpr bool is_valid = false;
        };

        template <>
        struct vertex_type<float> {
            using type                            = float;
            static constexpr unsigned int gl_type = GL_FLOAT;
            static constexpr std::size_t size     = 1;

            static constexpr bool is_valid = true;
        };

        template <>
        struct vertex_type<double> {
            using type                            = double;
            static constexpr unsigned int gl_type = GL_DOUBLE;
            static constexpr std::size_t size     = 1;

            static constexpr bool is_valid = true;
        };

        template <>
        struct vertex_type<glm::vec2> {
            using type                            = float;
            static constexpr unsigned int gl_type = GL_FLOAT;
            static constexpr std::size_t size     = 2;

            static constexpr bool is_valid = true;
        };

        template <>
        struct vertex_type<glm::vec3> {
            using type                            = float;
            static constexpr unsigned int gl_type = GL_FLOAT;
            static constexpr std::size_t size     = 3;

            static constexpr bool is_valid = true;
        };

        template <>
        struct vertex_type<glm::vec4> {
            using type                            = float;
            static constexpr unsigned int gl_type = GL_FLOAT;
            static constexpr std::size_t size     = 4;

            static constexpr bool is_valid = true;
        };
    }

    template <typename Type>
    using vertex_type = detail::vertex_type<Type>;

    template <typename Type>
    concept valid_vertex_type = detail::vertex_type<Type>::is_valid;
}

#endif // SGW_GL_VERTEX_TYPE_H