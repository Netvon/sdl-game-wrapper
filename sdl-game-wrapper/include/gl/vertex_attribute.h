#ifndef SGW_GL_VERTEX_ATTRIBUTE_H
#define SGW_GL_VERTEX_ATTRIBUTE_H

#include "vertex_type.h"

#include <type_traits>

namespace sgw {
    template <typename Type, std::enable_if_t<vertex_type<Type>::is_valid, int> = 0>
    struct vertex_attribute {
        using value_type = Type;
        using type       = typename detail::vertex_type<Type>::type;

        static constexpr unsigned int gl_type = vertex_type<Type>::gl_type;
        static constexpr std::size_t stride   = vertex_type<Type>::size * sizeof(type);
        static constexpr std::size_t size     = vertex_type<Type>::size;
        static constexpr bool normalized      = false;
    };
}
#endif // SGW_GL_VERTEX_ATTRIBUTE_H