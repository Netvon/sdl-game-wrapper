#ifndef SGW_GL_BUFFER_H
#define SGW_GL_BUFFER_H

#include <glad/glad.h>

namespace sgw {

    enum struct buffer_target : GLenum {
        array_buffer          = GL_ARRAY_BUFFER,
        atomic_counter        = GL_ATOMIC_COUNTER_BUFFER,
        copy_read             = GL_COPY_READ_BUFFER,
        copy_write            = GL_COPY_WRITE_BUFFER,
        dispatch_indirect     = GL_DISPATCH_INDIRECT_BUFFER,
        draw_indirect         = GL_DRAW_INDIRECT_BUFFER,
        element_array_buffer  = GL_ELEMENT_ARRAY_BUFFER,
        pixel_pack            = GL_PIXEL_PACK_BUFFER,
        pixel_unpack          = GL_PIXEL_UNPACK_BUFFER,
        query                 = GL_QUERY_BUFFER,
        shader_storage_buffer = GL_SHADER_STORAGE_BUFFER,
        texture               = GL_TEXTURE_BUFFER,
        transform_feedback    = GL_TRANSFORM_FEEDBACK_BUFFER,
        uniform               = GL_UNIFORM_BUFFER
    };

    enum struct buffer_usage : GLenum {
        stream_draw  = GL_STREAM_DRAW,
        stream_read  = GL_STREAM_READ,
        stream_copy  = GL_STREAM_COPY,
        static_draw  = GL_STATIC_DRAW,
        static_read  = GL_STATIC_READ,
        static_copy  = GL_STATIC_COPY,
        dynamic_draw = GL_DYNAMIC_DRAW,
        dynamic_read = GL_DYNAMIC_READ,
        dynamic_copy = GL_DYNAMIC_COPY
    };

    enum struct buffer_offset_method { offset_mul_size, offset };

    enum struct buffer_binding_policy { bind_and_unbind, nothing };

    template <buffer_target BufferTarget>
    struct tbuffer {
        using id                              = GLuint;
        using size_t                          = GLsizeiptr;
        using type                            = tbuffer<BufferTarget>;
        constexpr static buffer_target target = BufferTarget;
        constexpr static GLenum target_enum   = static_cast<GLenum>(target);

        using usage = buffer_usage;

        [[nodiscard]] tbuffer::id get_id() const noexcept { return m_buffer_id; }
        [[nodiscard]] bool get_is_valid() const noexcept { return glIsBuffer(m_buffer_id); }


        void bind() const { glBindBuffer(static_cast<GLenum>(target), m_buffer_id); }

        void unbind() const { glBindBuffer(static_cast<GLenum>(target), 0); }

        template <typename Data, buffer_offset_method offset_method = buffer_offset_method::offset_mul_size>
        void set_data(const Data& data, tbuffer::size_t offset = 0LLU, tbuffer::size_t size = sizeof(Data)) const {
            bind();
            if constexpr (offset_method == buffer_offset_method::offset_mul_size) { offset *= size; }

            glBufferSubData(static_cast<GLenum>(target), offset, size, &data);
            unbind();
        }

        template <typename Data, buffer_offset_method offset_method = buffer_offset_method::offset_mul_size>
        void get_data(Data& data, tbuffer::size_t offset = 0LLU, tbuffer::size_t size = sizeof(Data)) const {
            bind();

            if constexpr (offset_method == buffer_offset_method::offset_mul_size) { offset *= size; }

            glGetBufferSubData(static_cast<GLenum>(target), offset, size, &data);
            unbind();
        }

        template <typename Data, buffer_offset_method offset_method = buffer_offset_method::offset_mul_size>
        [[nodiscard]] Data get_data(tbuffer::size_t offset = 0LLU, tbuffer::size_t size = sizeof(Data)) const {
            Data temp;
            get_data<Data, offset_method>(offset, size);
            return temp;
        }

        [[nodiscard]] GLint get_size() const {
            GLint size = 0;
            glGetBufferParameteriv(target_enum, GL_BUFFER_SIZE, &size);
            return size;
        }

        [[nodiscard]] buffer_usage get_usage() const {
            GLint usage = GL_STATIC_DRAW;
            glGetBufferParameteriv(target_enum, GL_BUFFER_USAGE, &usage);
            return static_cast<buffer_usage>(usage);
        }

        // template <typename Data>
        // void update_data_from_container(const Data& data, tbuffer::size_t offset = 0LLU) const {
        //	bind();
        //	glBufferSubData(static_cast<GLenum>(target), offset * sizeof(Data::value_type), data.size() * sizeof(Data::value_type), data.data());
        //	unbind();
        //}

        // template <typename Data, buffer_binding_policy binding_policy = buffer_binding_policy::bind_and_unbind>
        // void set_data_from_container(const Data& data, tbuffer::size_t offset = 0LLU) const {
        //	using type = std::remove_const_t<std::remove_pointer_t<decltype(std::data(data))>>;

        //	bind<binding_policy>();
        //	glBufferData(static_cast<GLenum>(target), std::size(data) * sizeof(type), std::data(data), static_cast<GLenum>(usage));
        //	unbind<binding_policy>();
        //}

        void destroy() const {
            if (get_is_valid()) { glDeleteBuffers(1, &m_buffer_id); }
        }

        [[nodiscard]] static tbuffer from_id(tbuffer::id buffer_id) noexcept {
            tbuffer temp;
            temp.m_buffer_id = buffer_id;
            return temp;
        }

        template <typename Data>
        [[nodiscard]] static tbuffer make_buffer(const Data& data, tbuffer::size_t size, buffer_usage usage) {
            tbuffer::id buffer_id;
            glGenBuffers(1, &buffer_id);
            tbuffer buf = tbuffer::from_id(buffer_id);

            buf.bind();
            glBufferData(static_cast<GLenum>(target), size, &data, usage);
            buf.unbind();

            return buf;
        }

        template <typename Data>
        [[nodiscard]] static tbuffer make_buffer(const Data& data, buffer_usage usage) {
            return make_buffer(data, sizeof(Data), usage);
        }

        template <typename Data>
        [[nodiscard]] static tbuffer make_buffer_from_container(const Data& data, buffer_usage usage, bool unbind_after_create = true) {
            using type = std::remove_const_t<std::remove_pointer_t<decltype(std::data(data))>>;

            tbuffer::id buffer_id;
            glGenBuffers(1, &buffer_id);
            tbuffer buf = tbuffer::from_id(buffer_id);

            buf.bind();
            glBufferData(static_cast<GLenum>(target), std::size(data) * sizeof(type), std::data(data), static_cast<GLenum>(usage));

            if (unbind_after_create) { buf.unbind(); }

            return buf;
        }

    private:
        tbuffer::id m_buffer_id;
    };

    using shader_storage_buffer = tbuffer<buffer_target::shader_storage_buffer>;
    using element_array_buffer  = tbuffer<buffer_target::element_array_buffer>;
    using array_buffer          = tbuffer<buffer_target::array_buffer>;

    template <typename Buffer>
    void bind_buffer(const Buffer& buffer) {
        buffer.bind();
    }

    void bind_buffer(const shader_storage_buffer& buffer, GLuint index) {
        buffer.bind();
        glBindBufferBase(shader_storage_buffer::target_enum, index, buffer.get_id());
    }

    template <typename Buffer>
    void unbind_buffer(const Buffer& buffer) {
        buffer.unbind();
    }

    // struct shader_storage_buffer {
    //	using id = GLuint;
    //	constexpr static auto buffer_type = GL_SHADER_STORAGE_BUFFER;

    //	explicit shader_storage_buffer(id buffer_id) : m_id(buffer_id) {}
    //	shader_storage_buffer() = default;

    //	template <typename Data>
    //	static shader_storage_buffer make(const Data& data) {
    //		shader_storage_buffer temp;
    //		glGenBuffers(1, &temp.m_id);
    //		temp.bind();
    //		glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(Data::value_type), data.data(), GL_DYNAMIC_COPY);
    //		temp.unbind();

    //		return temp;
    //	}

    //	static void bind(id buffer_id) {
    //		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_id);
    //	}

    //	static void unbind() {
    //		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    //	}

    //	void bind() const {
    //		bind(m_id);
    //	}

    //	[[nodiscard]] id get_id() const noexcept { return m_id; }

    //	template <typename Data>
    //	void update_full(const Data& data) {
    //		bind();

    //		GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    //		memcpy(p, &data, data.size() * sizeof(Data::value_type));
    //		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    //	}

    //	template <typename Data>
    //	void update_sub(const Data& data, std::size_t offset, std::size_t size) {
    //		bind();
    //		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, &data);
    //		unbind();
    //	}

    //	template <typename Data>
    //	void update_sub(const Data& data) {
    //		update_sub(data, 0, data.size() * sizeof(Data::value_type));
    //	}

    // private:
    //	id m_id;
    //};
}
#endif