#pragma once
#include <SDL.h>
#include <glm/glm.hpp>

namespace netvon {
	[[nodiscard]] constexpr glm::vec2 convert(const SDL_FPoint& point) {
		return { point.x, point.y };
	}

	[[nodiscard]] constexpr glm::vec2 convert(SDL_FPoint&& point) {
		return { point.x, point.y };
	}

	[[nodiscard]] constexpr SDL_FPoint convert(const glm::vec2& point) {
		return { point.x, point.y };
	}

	[[nodiscard]] constexpr SDL_FPoint convert(glm::vec2&& point) {
		return { point.x, point.y };
	}
}