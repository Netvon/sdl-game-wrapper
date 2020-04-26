#include "../include/gl/context.h"
#include "../include/sdl/window.h"
#include <iostream>
#include <glad.c>

namespace sgw::gl {

	void APIENTRY debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
		std::cerr << source << " - " << type << " - " << id << " - " << severity << " - " << length << " - " << message;
	}

	context::context(const sdl::window& window) {
		m_gl_context = SDL_GL_CreateContext(window.m_window_ptr);

		if (m_gl_context == nullptr) {
			throw sdl::window_opengl_create_error();
		}

		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
			throw sdl::window_opengl_create_error();
		}

		auto gl_major = GLVersion.major;
		auto gl_minor = GLVersion.minor;

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_minor);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

#ifdef _DEBUG
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
		//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		// setup OpenGL to output all error messages using std::cerr
		glDebugMessageCallback(&debug_callback, nullptr);
		glDebugMessageControl(
			GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif // _DEBUG

		//glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
	}

	void context::make_current(const sdl::window& window) const {
		if (m_gl_context != nullptr) {
			SDL_GL_MakeCurrent(window.m_window_ptr, m_gl_context);
		}
	}

	void context::set_enable(unsigned int value) {
		glEnable(value);
	}


}
