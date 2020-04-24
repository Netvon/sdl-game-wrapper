#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string_view>
#include <map>
#include <utility>
#include "errors.h"
#include "font.h"

namespace sgw {

	enum class font_resource : std::size_t {};

	struct font_manager {
		using key = std::pair<std::string_view, int>;

		font_manager() = default;
		font_manager(const font_manager&) = delete;
		font_manager(font_manager&&) = delete;
		font_manager& operator=(const font_manager&) = delete;
		font_manager& operator=(font_manager&&) = delete;
		~font_manager() = default;

		[[nodiscard]] const sdl::lib_ttf& get_lib_ttf() const noexcept {
			return m_lib_ttf;
		}

		auto add_font(std::string_view path, std::string_view name, int point_size) {
			sdl::font f(path, point_size);
			auto key = std::make_pair(name, point_size);
			m_loaded_fonts.insert(std::make_pair(key, std::move(f)));
			return key;
		}

		[[nodiscard]] const sdl::font& get_font(std::string_view name, int point_size) const {
			auto& value = m_loaded_fonts.at(std::make_pair(name, point_size));
			return value;
		}

		[[nodiscard]] const sdl::font& get_font(key key) const {
			auto& value = m_loaded_fonts.at(key);
			return value;
		}

	private:
		sdl::lib_ttf m_lib_ttf;
		std::map<key, sdl::font> m_loaded_fonts;
	};
}