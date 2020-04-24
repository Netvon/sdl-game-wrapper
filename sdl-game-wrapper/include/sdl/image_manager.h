#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string_view>
#include <map>
#include <utility>
#include "surface.h"
#include "errors.h"
#include "lib_image.h"
#include "texture.h"

namespace sgw {

	enum class image_resource : std::size_t {};

	struct image_manager {
		using key = std::string_view;

		image_manager() = delete;
		explicit image_manager(Uint32 lib_image_flags) : m_lib_image(lib_image_flags) {}
		image_manager(const image_manager&) = delete;
		image_manager(image_manager&&) = delete;
		image_manager& operator=(const image_manager&) = delete;
		image_manager& operator=(image_manager&&) = delete;
		~image_manager() = default;

		[[nodiscard]] const sdl::lib_image& get_lib_image() const noexcept {
			return m_lib_image;
		}

		//[[nodiscard]] static sdl::surface load_image(std::string_view path) {
		//	return sdl::surface(IMG_Load(path.data()));
		//}

		[[nodiscard]] static sdl::surface load_image(const std::string& path) {
			return sdl::surface(IMG_Load(path.c_str()));
		}

		image_resource add_image(std::string_view path) {
			image_resource img{ m_images_amount };
			sdl::surface s(IMG_Load(path.data()));
			m_loaded_images.insert(std::make_pair(img, std::move(s)));
			m_images_amount++;
			return img;
		}

		[[nodiscard]] const sdl::surface& get_image(image_resource name) const {
			return m_loaded_images.at(name);
		}

	private:
		sdl::lib_image m_lib_image;
		std::map<image_resource, sdl::surface> m_loaded_images;
		std::size_t m_images_amount{ 0 };
	};
}