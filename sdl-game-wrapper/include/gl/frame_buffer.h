#ifndef SGW_GL_FRAME_BUFFER_H
#define SGW_GL_FRAME_BUFFER_H

#include "texture.h"

#include <glad/glad.h>
#include <stdexcept>
#include <string>

namespace sgw {
    struct frame_buffer {
        using id = GLuint;

        explicit frame_buffer(id buffer_id)
            : m_id(buffer_id) {}
        frame_buffer() = default;

        static frame_buffer make(int width, int height) {
            frame_buffer temp;
            glGenFramebuffers(1, &temp.m_id);
            glBindFramebuffer(GL_FRAMEBUFFER, temp.m_id);

            temp.m_texture = texture2d::create(width, height, texture_filtering::linear, texture_format::rgb);
            temp.m_texture.bind();

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, temp.m_texture.get_id(), 0);

            glGenRenderbuffers(1, &temp.m_render_buffer);
            glBindRenderbuffer(GL_RENDERBUFFER, temp.m_render_buffer);
            glRenderbufferStorage(
                GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, temp.m_render_buffer); // now actually attach it

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            temp.m_texture.unbind();

            return temp;
        }

        static void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

        void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_id); }

        void destroy() const { glDeleteFramebuffers(1, &m_id); }

        [[nodiscard]] id get_id() const noexcept { return m_id; }
        [[nodiscard]] id get_render_buffer() const noexcept { return m_render_buffer; }
        [[nodiscard]] const texture2d& get_texture() const noexcept { return m_texture; }

    private:
        id m_id            = 0;
        id m_render_buffer = 0;
        texture2d m_texture;
    };
}
#endif // SGW_GL_FRAME_BUFFER_H