#include "../include/gl/context.h"

#include "../include/sdl/window.h"

#include <glad/glad.c>
#include <iostream>

namespace sgw {
#ifdef _DEBUG
    namespace gldebug {
        const char* get_source_name(GLenum source) {
            switch (source) {
            case GL_DEBUG_SOURCE_API:
                return "SOURCE_API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                return "SOURCE_WINDOW_SYSTEM";
            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                return "SOURCE_SHADER_COMPILER";
            case GL_DEBUG_SOURCE_THIRD_PARTY:
                return "SOURCE_THIRD_PARTY";
            case GL_DEBUG_SOURCE_APPLICATION:
                return "SOURCE_APPLICATION";
            default:
                return "SOURCE_OTHER";
            }
        }

        const char* get_type_name(GLenum type) {
            switch (type) {
            case GL_DEBUG_TYPE_ERROR:
                return "TYPE_ERROR";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                return "TYPE_DEPRECATED_BEHAVIOR";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                return "TYPE_UNDEFINED_BEHAVIOR";
            case GL_DEBUG_TYPE_PORTABILITY:
                return "TYPE_PORTABILITY";
            case GL_DEBUG_TYPE_PERFORMANCE:
                return "TYPE_PERFORMANCE";
            case GL_DEBUG_TYPE_MARKER:
                return "TYPE_MARKER";
            case GL_DEBUG_TYPE_PUSH_GROUP:
                return "TYPE_PUSH_GROUP";
            case GL_DEBUG_TYPE_POP_GROUP:
                return "TYPE_POP_GROUP";
            default:
                return "TYPE_OTHER";
            }
        }

        const char* get_severity_name(GLenum severity) {
            switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                return "SEVERITY_HIGH";
            case GL_DEBUG_SEVERITY_MEDIUM:
                return "SEVERITY_MEDIUM";
            case GL_DEBUG_SEVERITY_LOW:
                return "SEVERITY_LOW";
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                return "SEVERITY_NOTIFICATION";
            default:
                return "SEVERITY_OTHER";
            }
        }
    }

    void APIENTRY
    debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {

        std::cerr << "GL Debug: "
                  << "[" << gldebug::get_severity_name(severity) << "]"
                  << "[" << gldebug::get_source_name(source) << "]"
                  << "[" << gldebug::get_type_name(type) << "]"
                  << "[ID:" << id << "]\n"
                  << "GL Debug: " << message << '\n';
    }
#endif // _DEBUG

    context::context(const sdl::window& window) {

#ifdef _DEBUG
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
        // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif // _DEBUG

        m_gl_context = SDL_GL_CreateContext(window.m_window_ptr);

        if (m_gl_context == nullptr) { throw sdl::window_opengl_create_error(); }

        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) { throw sdl::window_opengl_create_error(); }

        auto gl_major = GLVersion.major;
        auto gl_minor = GLVersion.minor;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_minor);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

#ifdef _DEBUG
        // setup OpenGL to output all error messages using std::cerr
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debug_callback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif // _DEBUG

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        if (SDL_GL_SetSwapInterval(1) == -1) { std::cerr << "Set vsync failed" << '\n'; }

        if (SDL_GL_MakeCurrent(window.m_window_ptr, m_gl_context)) { std::cerr << "MakeCurrent Failed" << '\n'; }

        glEnable(GL_BLEND); // you enable blending function
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        const GLubyte* renderer = glGetString(GL_RENDERER);
        const GLubyte* version  = glGetString(GL_VERSION);

        printf("Renderer: %s\n", renderer);
        printf("OpenGL version %s\n", version);
    }

    void context::make_current(const sdl::window& window) const {
        if (m_gl_context != nullptr) { SDL_GL_MakeCurrent(window.m_window_ptr, m_gl_context); }
    }

    void context::set_enable(unsigned int value) { glEnable(value); }


}
