#ifndef SGW_GL_TEXTURE_H
#define SGW_GL_TEXTURE_H
#include <glad/glad.h>
#include <stb_image.h>
#include <stdexcept>
#include <string>

namespace sgw {

    struct texture_load_error : public std::runtime_error {
        explicit texture_load_error(const std::string& error)
            : std::runtime_error(error) {}
    };

    enum struct texture_target : GLenum {
        texture_2d              = GL_TEXTURE_2D,
        proxy_texture_2d        = GL_PROXY_TEXTURE_2D,
        texture_1d_array        = GL_TEXTURE_1D_ARRAY,
        proxy_texture_1d_array  = GL_PROXY_TEXTURE_1D_ARRAY,
        texture_rectangle       = GL_TEXTURE_RECTANGLE,
        proxy_texture_rectangle = GL_PROXY_TEXTURE_RECTANGLE,
        cube_map_positive_x     = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        cube_map_negative_x     = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        cube_map_positive_y     = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        cube_map_negative_y     = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        cube_map_positive_z     = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        cube_map_negative_z     = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
        proxy_texture_cube_map  = GL_PROXY_TEXTURE_CUBE_MAP,

        texture_3d             = GL_TEXTURE_3D,
        proxy_texture_3d       = GL_PROXY_TEXTURE_3D,
        texture_2d_array       = GL_TEXTURE_2D_ARRAY,
        proxy_texture_2d_array = GL_PROXY_TEXTURE_2D_ARRAY
    };

    enum struct texture_format : GLint {
        depth_component = GL_DEPTH_COMPONENT,
        depth_stencil   = GL_DEPTH_STENCIL,
        red             = GL_RED,
        rg              = GL_RG,
        rgb             = GL_RGB,
        rgba            = GL_RGBA
    };

    enum struct texture_filtering { linear, nearest };

    enum struct texture_mipmap { enable, disable };

    template <texture_target TextureTarget>
    struct ttexture {
        using id   = unsigned int;
        using type = ttexture<TextureTarget>;

        static constexpr texture_target texture_target = TextureTarget;
        static constexpr GLenum texture_target_gl      = static_cast<GLenum>(TextureTarget);
        static constexpr bool is_2d_target = texture_target != texture_target::texture_3d && texture_target != texture_target::proxy_texture_3d &&
                                             texture_target != texture_target::texture_2d_array &&
                                             texture_target != texture_target::proxy_texture_2d_array;

        static ttexture create() {
            ttexture temp;

            glGenTextures(1, &temp.m_image);
            return temp;
        }

        static ttexture create_from_file(const std::string& path,
                                         texture_filtering filtering = texture_filtering::linear,
                                         texture_mipmap mipmap       = texture_mipmap::enable) {
            // stbi_set_flip_vertically_on_load(true);

            ttexture temp = create();
            temp.bind();

            glTexParameteri(texture_target_gl, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(texture_target_gl, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            auto filter = GL_LINEAR;
            if (filtering == texture_filtering::nearest) { filter = GL_NEAREST; }

            glTexParameteri(texture_target_gl, GL_TEXTURE_MAG_FILTER, filter);

            int width;
            int height;
            int channels;
            auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);

            if (data != nullptr) {
                glTexImage2D(texture_target_gl, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

                if (mipmap == texture_mipmap::enable) {
                    if (filter == GL_NEAREST) { filter = GL_NEAREST_MIPMAP_NEAREST; }

                    glTexParameteri(texture_target_gl, GL_TEXTURE_MIN_FILTER, filter);
                    glGenerateMipmap(GL_TEXTURE_2D);
                }
                else {
                    glTexParameteri(texture_target_gl, GL_TEXTURE_MIN_FILTER, filter);
                }

                stbi_image_free(data);
            }
            else {
                throw texture_load_error(stbi_failure_reason());
            }

            temp.unbind();
            return temp;
        }

        static ttexture
        create(int width, int height, texture_filtering filtering = texture_filtering::linear, texture_format format = texture_format::rgba) {
            static_assert(is_2d_target, "Only a 2D Texture can be created without a depth component");

            ttexture temp = create();
            temp.bind();

            auto tex_format = static_cast<GLint>(format);
            auto filter = GL_LINEAR;
            if (filtering == texture_filtering::nearest) { filter = GL_NEAREST; }

            glTexParameteri(texture_target_gl, GL_TEXTURE_MIN_FILTER, filter);
            glTexParameteri(texture_target_gl, GL_TEXTURE_MAG_FILTER, filter);

            glTexImage2D(texture_target_gl, 0, tex_format, width, height, 0, tex_format, GL_UNSIGNED_BYTE, nullptr);

            return temp;
        }

        // static ttexture create(int width, int height, int depth) {
        //	static_assert(is_2d_target == false, "Only a 3D Texture can be created with a depth component");

        //	ttexture temp = create();
        //	temp.bind();

        //	glTexParameteri(texture_target_gl, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //	glTexParameteri(texture_target_gl, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //	glTexImage3D(texture_target_gl, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        //	return temp;
        //}

        void bind() const { glBindTexture(GL_TEXTURE_2D, m_image); }

        void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

        [[nodiscard]] id get_id() const noexcept { return m_image; }

    private:
        id m_image{};
    };

    using texture   = ttexture<texture_target::texture_2d>;
    using texture2d = ttexture<texture_target::texture_2d>;
}
#endif // SGW_GL_TEXTURE_H