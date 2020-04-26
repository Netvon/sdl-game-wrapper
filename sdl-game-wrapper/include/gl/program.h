#pragma once
#include <glad.h>

namespace sdl::gl {
	struct shader {

	};

	struct program {
		using id = GLint;

		program() {
			m_program = glCreateProgram();
		}

		explicit program(id program) : m_program(program) {}

		program(const program&) noexcept = default;
		program(program&&) noexcept = default;
		program& operator=(program&&) noexcept = default;
		program& operator=(const program&) noexcept = default;

		void attach_shader(const shader& shader) const {

		}

		void link() const {
			glLinkProgram(m_program);
		}

		static void link(id program) {
			glLinkProgram(program);
		}

		void use() const {
			glUseProgram(m_program);
		}

		static void use(id program) {
			glUseProgram(program);
		}

		~program() {
			glDeleteProgram(m_program);
		}

		[[nodiscard]] bool is_valid() const {
			return glIsProgram(m_program) == GL_TRUE;
		}

	private:
		id m_program{};
	};
}