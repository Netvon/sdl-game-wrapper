#ifndef SGW_GL_FRAME_BUFFER_H
#define SGW_GL_FRAME_BUFFER_H

#include "texture.h"

#include <glad/glad.h>
#include <stdexcept>
#include <string>

namespace sgw {
    enum struct frame_buffer_status : GLenum {
        error                         = 0,
        complete                      = GL_FRAMEBUFFER_COMPLETE,
        undefined                     = GL_FRAMEBUFFER_UNDEFINED,
        incomplete_attachment         = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
        incomplete_missing_attachment = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
        incomplete_draw_buffer        = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,
        incomplete_read_buffer        = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,
        unsupported                   = GL_FRAMEBUFFER_UNSUPPORTED,
        incomplete_multisample        = GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,
        incomplete_layer_targets      = GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
    };

    enum struct frame_buffer_type : GLenum { normal = GL_FRAMEBUFFER, read = GL_READ_FRAMEBUFFER, draw = GL_DRAW_FRAMEBUFFER };

    struct frame_buffer {
        using id = GLuint;

        explicit frame_buffer(id buffer_id)
            : m_id(buffer_id) {}
        frame_buffer() = default;

        static frame_buffer make(int width, int height, frame_buffer_type type = frame_buffer_type::normal) {
            frame_buffer temp;
            temp.m_type = type;

            auto target = temp.get_target();
            bool same = target == GL_FRAMEBUFFER;

            glGenFramebuffers(1, &temp.m_id);
            glBindFramebuffer(target, temp.m_id);

            temp.m_texture = texture2d::create(width, height, texture_filtering::linear, texture_format::rgba);
            temp.m_texture.bind();

            glFramebufferTexture2D(target, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, temp.m_texture.get_id(), 0);

            glGenRenderbuffers(1, &temp.m_render_buffer);
            glBindRenderbuffer(GL_RENDERBUFFER, temp.m_render_buffer);

            glRenderbufferStorage(
                GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
            glFramebufferRenderbuffer(target, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, temp.m_render_buffer); // now actually attach it

            temp.unbind();
            temp.m_texture.unbind();

            return temp;
        }

        void unbind() const noexcept { glBindFramebuffer(get_target(), 0); }

        void bind() const noexcept { glBindFramebuffer(get_target(), m_id); }

        void destroy() const noexcept { glDeleteFramebuffers(1, &m_id); }

        [[nodiscard]] id get_id() const noexcept { return m_id; }
        [[nodiscard]] id get_render_buffer() const noexcept { return m_render_buffer; }
        [[nodiscard]] const texture2d& get_texture() const noexcept { return m_texture; }
        [[nodiscard]] bool is_valid() const {
            if (glIsFramebuffer(m_id) != GL_TRUE || glIsBuffer(m_render_buffer) != GL_TRUE || !m_texture.is_valid()) { return false; }
            return get_status() == frame_buffer_status::complete;
        }

        [[nodiscard]] frame_buffer_status get_status() const noexcept {
            bind();
            auto result = static_cast<frame_buffer_status>(glCheckFramebufferStatus(get_target()));
            frame_buffer::unbind();

            return result;
        }

        [[nodiscard]] GLenum get_target() const noexcept { return static_cast<GLenum>(m_type); }

    private:
        id m_id                  = 0;
        id m_render_buffer       = 0;
        frame_buffer_type m_type = frame_buffer_type::normal;
        texture2d m_texture;
    };
}
#endif // SGW_GL_FRAME_BUFFER_H