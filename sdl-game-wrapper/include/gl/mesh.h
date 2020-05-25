#ifndef SGW_GL_MESH_H
#define SGW_GL_MESH_H

#include "vertex_layout.h"

#include <array>
#include <cstddef>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <initializer_list>
#include <type_traits>

namespace sgw {

    enum struct mesh_type { array, elements };

    struct mesh {

        enum struct after_create_operation { unbind_buffer, nothing };

        enum struct draw_operation_buffer_handling { automatic_bind_and_unbind, manual_bind_and_unbind };

        mesh() = default;

        template <typename VertexList, typename... VertexAttribute>
        mesh(const VertexList& vertices,
             const vertex_layout<VertexAttribute...>& layout,
             mesh::after_create_operation after_create = mesh::after_create_operation::unbind_buffer)
            : m_vertex_amount(std::size(vertices))
            , m_element_amount(0)
            , m_has_elements(false) {
            static_assert(vertex_layout<VertexAttribute...>::is_layout);

            using vertex_type = std::remove_const_t<std::remove_pointer_t<decltype(std::data(vertices))>>;

            static_assert(std::is_same_v<vertex_type, float>);

            // create the vertex array and buffers to store the vertices/elements
            glGenVertexArrays(1, &m_vertex_array);
            glGenBuffers(1, &m_vertex_array_buffer);
            glBindVertexArray(m_vertex_array);

            auto vertex_start = std::data(vertices);
            set_array_buffer_data(vertex_start);

            vertex_layout<VertexAttribute...>::set_attributes();

            if (after_create == mesh::after_create_operation::unbind_buffer) { unbind(); }
        }

        template <typename VertexList, typename IndexList, typename... VertexAttribute>
        mesh(const VertexList& vertices,
             const IndexList& indices,
             const vertex_layout<VertexAttribute...>& layout,
             mesh::after_create_operation after_create = mesh::after_create_operation::unbind_buffer)
            : m_vertex_amount(std::size(vertices))
            , m_element_amount(std::size(indices))
            , m_has_elements(true) {
            static_assert(vertex_layout<VertexAttribute...>::is_layout);

            using vertex_type = std::remove_const_t<std::remove_pointer_t<decltype(std::data(vertices))>>;
            using index_type  = std::remove_const_t<std::remove_pointer_t<decltype(std::data(indices))>>;

            static_assert(std::is_same_v<vertex_type, float>);
            static_assert(std::is_same_v<index_type, unsigned int>);

            // create the vertex array and buffers to store the vertices/elements
            glGenVertexArrays(1, &m_vertex_array);
            glGenBuffers(1, &m_vertex_array_buffer);
            glGenBuffers(1, &m_element_array_buffer);

            auto vertex_start = std::data(vertices);
            auto elemen_start = std::data(indices);

            glBindVertexArray(m_vertex_array);

            set_array_buffer_data(vertex_start);
            set_element_buffer_data(elemen_start);

            vertex_layout<VertexAttribute...>::set_attributes();

            if (after_create == mesh::after_create_operation::unbind_buffer) { unbind(); }
        }

        void bind() const {
            glBindVertexArray(m_vertex_array);
            glBindBuffer(GL_ARRAY_BUFFER, m_vertex_array_buffer);

            if (m_has_elements) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_element_array_buffer); }
        }

        void unbind() const {
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            if (m_has_elements) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

            glBindVertexArray(0);
        }

        [[nodiscard]] bool has_elements() const noexcept { return m_has_elements; }

        [[nodiscard]] GLsizei get_vertex_amount() const noexcept { return m_vertex_amount; }

        [[nodiscard]] GLsizei get_element_amount() const noexcept { return m_element_amount; }

        void draw(draw_operation_buffer_handling buffer_handling = draw_operation_buffer_handling::automatic_bind_and_unbind,
                  GLenum mode                                    = GL_TRIANGLES) {

            if (buffer_handling == draw_operation_buffer_handling::automatic_bind_and_unbind) { bind(); }

            if (has_elements()) { glDrawElements(mode, get_element_amount(), GL_UNSIGNED_INT, nullptr); }
            else {
                glDrawArrays(mode, 0, get_vertex_amount());
            }

            if (buffer_handling == draw_operation_buffer_handling::automatic_bind_and_unbind) { unbind(); }
        }

        void draw_instanced(GLsizei instance_count,
                            draw_operation_buffer_handling buffer_handling = draw_operation_buffer_handling::automatic_bind_and_unbind,
                            GLenum mode                                    = GL_TRIANGLES) {
            if (buffer_handling == draw_operation_buffer_handling::automatic_bind_and_unbind) { bind(); }

            if (has_elements()) { glDrawElementsInstanced(mode, get_element_amount(), GL_UNSIGNED_INT, nullptr, instance_count); }
            else {
                glDrawArraysInstanced(mode, 0, get_vertex_amount(), instance_count);
            }

            if (buffer_handling == draw_operation_buffer_handling::automatic_bind_and_unbind) { unbind(); }
        }

    private:
        GLuint m_vertex_array         = 0;
        GLuint m_vertex_array_buffer  = 0;
        GLuint m_element_array_buffer = 0;

        std::size_t m_vertex_amount  = 0;
        std::size_t m_element_amount = 0;

        bool m_has_elements = false;

        void set_array_buffer_data(const void* data) const {
            glBindBuffer(GL_ARRAY_BUFFER, m_vertex_array_buffer);
            glBufferData(GL_ARRAY_BUFFER, m_vertex_amount * sizeof(float), data, GL_STATIC_DRAW);
        }

        void set_element_buffer_data(const void* data) const {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_element_array_buffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_element_amount * sizeof(unsigned int), data, GL_STATIC_DRAW);
        }
    };
}
#endif // SGW_GL_MESH_H