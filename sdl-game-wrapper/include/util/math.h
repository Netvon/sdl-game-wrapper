#pragma once
#include <glm/glm.hpp>
#include <optional>

namespace sgw::math {

	constexpr const glm::vec2 up{ 0.F, -1.F };
	constexpr const glm::vec2 down{ 0.F, 1.F };
	constexpr const glm::vec2 left{ -1.F, 0.F };
	constexpr const glm::vec2 right{ 1.F, 0.F };

	constexpr std::optional<glm::vec2> intersection(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& position, const glm::vec2& direction) {
		const auto x1 = p1.x;
		const auto y1 = p1.y;
		const auto x2 = p2.x;
		const auto y2 = p2.y;

		const auto x3 = position.x;
		const auto y3 = position.y;
		const auto x4 = position.x + direction.x;
		const auto y4 = position.y + direction.y;

		const auto den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		if (den == 0) {
			return std::nullopt;
		}

		const auto t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
		const auto u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;
		if (t > 0 && t < 1 && u > 0) {
			const glm::vec2 pt{ x1 + t * (x2 - x1), y1 + t * (y2 - y1) };
			return pt;
		}

		return std::nullopt;
	}

	constexpr bool in_rect(const glm::vec2& p, const glm::vec4& r) {
		return ((p.x >= r.x) && (p.x < (r.x + r.z)) &&
				(p.y >= r.y) && (p.y < (r.y + r.w)));
	}

	template<typename Vector>
	constexpr Vector limit_length(const Vector& vector, float max_length) {
		float length = glm::length(vector);

		if (length > max_length) {
			return glm::normalize(vector) * max_length;
		}

		return vector;
	}
}