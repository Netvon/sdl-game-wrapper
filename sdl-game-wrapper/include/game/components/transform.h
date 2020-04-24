#pragma once
#include <glm/glm.hpp>

namespace sgw::components {
	template<typename T, std::size_t Axis>
	struct generic_transform {

		using type = generic_transform<T, Axis>;
		using value_type = T;
		using vector_type = glm::vec<Axis, T, glm::packed_highp>;

		constexpr generic_transform() = default;
		explicit constexpr generic_transform(vector_type position) : m_position(position) {}
		constexpr generic_transform(value_type x, value_type y) : m_position(x, y) {}
		constexpr generic_transform(value_type x, value_type y, value_type rotation) : m_position(x, y), m_rotation(rotation) {}
		constexpr generic_transform(vector_type position, value_type rotation) : m_position(position), m_rotation(rotation) {}
		constexpr generic_transform(generic_transform&&) noexcept = default;
		constexpr generic_transform(const generic_transform&) = default;
		constexpr generic_transform& operator=(const generic_transform&) = default;
		constexpr generic_transform& operator=(generic_transform&&) noexcept = default;
		~generic_transform() = default;

		[[nodiscard]] constexpr const vector_type& get_position() const noexcept {
			return m_position;
		}

		constexpr void set_position(value_type x, value_type y) noexcept {
			m_position.x = x;
			m_position.y = y;
		}

		template <typename VectorType = vector_type>
		constexpr void set_position(const VectorType& position) noexcept {
			m_position.x = position.x;
			m_position.y = position.y;
		}

		template <typename VectorType = vector_type>
		constexpr void add_position(const VectorType& position) noexcept {
			m_position += position;
		}

		template <typename VectorType = vector_type>
		constexpr void subtract_position(const VectorType& position) noexcept {
			m_position -= position;
		}

		[[nodiscard]] constexpr const value_type& get_rotation() const noexcept {
			return m_rotation;
		}

		constexpr void set_rotation(value_type rotation) noexcept {
			m_rotation = rotation;
		}

		constexpr void add_rotation(value_type rotation) noexcept {
			m_rotation += rotation;
		}

		constexpr void subtract_rotation(value_type rotation) noexcept {
			m_rotation -= rotation;
		}

	private:
		vector_type m_position{};
		value_type m_rotation{};
	};

	using transform2d = generic_transform<float, 2>;
}