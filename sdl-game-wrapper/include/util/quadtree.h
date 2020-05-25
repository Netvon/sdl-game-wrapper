#pragma once
#include <glm/glm.hpp>
#include <array>
#include <memory>
#include <algorithm>

namespace sgw::util {

	struct quadtree {
		constexpr static std::size_t size = 10;

		explicit quadtree(glm::vec4 boundary) : m_boundary(boundary) {}

		void insert(const glm::vec2& value) {
			insert_impl(value);
		}

		template<typename Func>
		void for_each(Func func) const {
			std::for_each_n(std::begin(m_storage), m_stored_amount, func);

			if (m_is_subdivided) {
				m_north_east->for_each(func);
				m_south_east->for_each(func);
				m_south_west->for_each(func);
				m_north_west->for_each(func);
			}
		}

		template <typename Func>
		void for_each_boundary(Func func) const {
			func(m_boundary);

			if (m_is_subdivided) {
				m_north_east->for_each_boundary(func);
				m_south_east->for_each_boundary(func);
				m_south_west->for_each_boundary(func);
				m_north_west->for_each_boundary(func);
			}
		}

	private:
		std::array<glm::vec2, size> m_storage{};
		std::size_t m_stored_amount{ 0 };
		glm::vec4 m_boundary;

		std::unique_ptr<quadtree> m_north_east{ nullptr };
		std::unique_ptr<quadtree> m_south_east{ nullptr };
		std::unique_ptr<quadtree> m_south_west{ nullptr };
		std::unique_ptr<quadtree> m_north_west{ nullptr };

		bool m_is_subdivided = false;

		bool insert_impl(const glm::vec2& value) {
			if (!accept(value)) {
				return false;
			}

			if (m_stored_amount < size) {
				m_storage.at(m_stored_amount) = value;
				m_stored_amount++;
				return false;
			}

			subdivide_if_needed();

			return m_north_east->insert_impl(value) ||
				   m_south_east->insert_impl(value) ||
				   m_south_west->insert_impl(value) ||
				   m_north_west->insert_impl(value);
		}

		[[nodiscard]] bool accept(const glm::vec2& value) const {
			const auto boundary_position	= glm::vec2(m_boundary[0], m_boundary[1]);
			const auto boundary_size		= glm::vec2(m_boundary[2], m_boundary[3]);

			auto min = boundary_position - boundary_size;
			auto max = boundary_position + boundary_size;

			return value[0] >= min[0] &&
				   value[0] <= max[0] &&
				   value[1] >= min[1] &&
				   value[1] <= max[1];
		}

		void subdivide_if_needed() {
			if (m_is_subdivided) {
				return;
			}

			const auto boundary_position	= glm::vec2(m_boundary[0], m_boundary[1]);
			const auto boundary_size		= glm::vec2(m_boundary[2], m_boundary[3]);

			const auto bs = boundary_size * 0.5F;
			const auto ne = glm::vec4(boundary_position + bs * glm::vec2( 1.F, -1.F), bs);
			const auto se = glm::vec4(boundary_position + bs * glm::vec2( 1.F,  1.F), bs);
			const auto sw = glm::vec4(boundary_position + bs * glm::vec2(-1.F,  1.F), bs);
			const auto nw = glm::vec4(boundary_position + bs * glm::vec2(-1.F, -1.F), bs);

			m_north_east = std::make_unique<quadtree>(ne);
			m_south_east = std::make_unique<quadtree>(se);
			m_south_west = std::make_unique<quadtree>(sw);
			m_north_west = std::make_unique<quadtree>(nw);

			m_is_subdivided = true;
		}
	};
}