#ifndef SGW_GL_VERTEX_LAYOUT_H
#define SGW_GL_VERTEX_LAYOUT_H

#include "vertex_attribute.h"

namespace sgw {
    template <valid_vertex_type... VertexAttribute>
    struct vertex_layout {
        constexpr static bool is_valid      = (vertex_type<VertexAttribute>::is_valid && ...);
        constexpr static bool is_layout     = is_valid;
        constexpr static std::size_t stride = (vertex_attribute<VertexAttribute>::stride + ...);
        constexpr static std::size_t amount = (vertex_attribute<VertexAttribute>::size + ...);
        constexpr static std::size_t count  = sizeof...(VertexAttribute);

        static_assert(is_valid, "All Vertex Attributes must be valid, see vertex_type<T>");

        static void set_attributes(unsigned int initial_index = 0, std::size_t initial_offset = 0) {
            unsigned int index         = initial_index;
            std::size_t pointer_offset = initial_offset;

            (set_attribute<VertexAttribute>(index, pointer_offset), ...);
        }

    private:
        template <typename Attribute>
        static void set_attribute(unsigned int& index, std::size_t& pointer_offset) {

            using attrib = vertex_attribute<Attribute>;

            glVertexAttribPointer(index, static_cast<GLint>(attrib::size), attrib::gl_type, attrib::normalized, stride, (void*)pointer_offset);
            glEnableVertexAttribArray(index);

            index++;
            pointer_offset += attrib::stride;
        }
    };

    namespace vertex {
        template <typename... VertexAttribute>
        [[maybe_unused]] constexpr static vertex_layout<VertexAttribute...> layout{};
    }
}

#endif // SGW_GL_VERTEX_LAYOUT_H