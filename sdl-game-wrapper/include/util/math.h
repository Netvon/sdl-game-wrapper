#ifndef SGW_UTIL_MATH_H
#define SGW_UTIL_MATH_H
#include <glm/glm.hpp>
#include <optional>

namespace sgw::math {

    [[maybe_unused]] constexpr const glm::vec2 up{ 0.F, -1.F };
    [[maybe_unused]] constexpr const glm::vec2 down{ 0.F, 1.F };
    [[maybe_unused]] constexpr const glm::vec2 left{ -1.F, 0.F };
    [[maybe_unused]] constexpr const glm::vec2 right{ 1.F, 0.F };

    constexpr std::optional<glm::vec2> intersection(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& position, const glm::vec2& direction) {
        const auto x1 = p1[0];
        const auto y1 = p1[1];
        const auto x2 = p2[0];
        const auto y2 = p2[1];

        const auto x3 = position[0];
        const auto y3 = position[1];
        const auto x4 = position[0] + direction[0];
        const auto y4 = position[1] + direction[1];

        const auto den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        if (den == 0) { return std::nullopt; }

        const auto t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
        const auto u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;
        if (t > 0 && t < 1 && u > 0) {
            const glm::vec2 pt{ x1 + t * (x2 - x1), y1 + t * (y2 - y1) };
            return pt;
        }

        return std::nullopt;
    }

    constexpr bool in_rect(const glm::vec2& p, const glm::vec4& r) {
        return ((p[0] >= r[0]) && (p[0] < (r[0] + r[2])) && (p[1] >= r[1]) && (p[1] < (r[1] + r[3])));
    }

    template <typename Vector>
    constexpr Vector limit_length(const Vector& vector, float max_length) {
        float length = glm::length(vector);

        if (length > max_length) { return glm::normalize(vector) * max_length; }

        return vector;
    }
}
#endif // SGW_UTIL_MATH_H