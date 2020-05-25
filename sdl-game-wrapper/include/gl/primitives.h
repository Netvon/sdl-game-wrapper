#ifndef SGW_GL_PRIMITIVES_H
#define SGW_GL_PRIMITIVES_H

#include "mesh.h"

#include <array>

namespace sgw::primitives {
    namespace detail {
        constexpr static std::array quad_vertices{
            1.0F,  1.0F,  1.0F, 1.0F, // top right
            1.0F,  -1.0F, 1.0F, 0.0F, // bottom right
            -1.0F, -1.0F, 0.0F, 0.0F, // bottom left
            -1.0F, 1.0F,  0.0F, 1.0F, // top left
        };

        constexpr static std::array quad_elements{ 0U, 1U, 3U, 1U, 2U, 3U };

        constexpr static auto quad_layout = vertex::layout<glm::vec2, glm::vec2>;
    }

    mesh make_quad(mesh::after_create_operation after_create = mesh::after_create_operation::unbind_buffer) {
        return mesh(detail::quad_vertices, detail::quad_elements, detail::quad_layout, after_create);
    }
}

#endif // SGW_GL_PRIMITIVES_H