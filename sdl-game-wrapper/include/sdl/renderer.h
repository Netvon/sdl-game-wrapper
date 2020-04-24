#pragma once
#include <SDL.h>
#include <string_view>
#include "font.h"
#include "errors.h"
#include "window.h"

namespace sdl {
	template<typename PointType>
	struct extends {
		using point = PointType;

		point left_upper{ 0, 0 };
		point right_upper{ 0, 0 };
		point left_lower{ 0, 0 };
		point right_lower{ 0, 0 };
	};

	struct texture;
	struct renderer {

		enum class flags : unsigned int {
			software = 0x00000001U,
			accelerated = 0x00000002U,
			present_vsync = 0x00000004U,
			target_texture = 0x00000008U
		};

		renderer() = delete;
		renderer(const renderer&) = delete;
		renderer(renderer&& other) noexcept {
			std::swap(m_renderer_ptr, other.m_renderer_ptr);
		}

		renderer& operator=(const renderer&) = delete;
		renderer& operator=(renderer&& other) noexcept {
			std::swap(m_renderer_ptr, other.m_renderer_ptr);
			return *this;
		}

		renderer(const window& window, int index, Uint32 flags) {
			auto id = window.get_id();
			auto sdl_window = SDL_GetWindowFromID(id);

			if (sdl_window == nullptr) {
				throw unknown_window_error();
			}

			m_renderer_ptr = SDL_CreateRenderer(sdl_window, index, flags);
			if (m_renderer_ptr == nullptr) {
				throw renderer_create_error();
			}
		}

		void set_draw_color(const SDL_Color& color) const {
			if (SDL_SetRenderDrawColor(m_renderer_ptr, color.r, color.g, color.b, color.a) == -1) {
				throw renderer_draw_color_error();
			}
		}

		void set_draw_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const {
			if (SDL_SetRenderDrawColor(m_renderer_ptr, r, g, b, a) == -1) {
				throw renderer_draw_color_error();
			}
		}

		[[nodiscard]] SDL_Color get_draw_color() const {
			SDL_Color c;
			if (SDL_GetRenderDrawColor(m_renderer_ptr, &c.r, &c.g, &c.b, &c.a) == -1) {
				throw renderer_draw_color_error(); 
			}

			return c;
		}

		template<typename IntegerPointType = std::pair<int, int>>
		[[nodiscard]] IntegerPointType get_output_size() const {
			int w, h;

			SDL_GetRendererOutputSize(m_renderer_ptr, &w, &h);
			return { w, h };
		}

		template <typename FloatPointType = std::pair<float, float>>
		[[nodiscard]] FloatPointType get_output_size_f() const {
			auto [w, h] = get_output_size();
			return { static_cast<float>(w), static_cast<float>(h) };
		}

		template <typename IntegerPointType = std::pair<float, float>>
		[[nodiscard]] IntegerPointType get_output_size_extends() const {
			auto [w, h] = get_output_size();
			return {
				.left_upper		= { 0, 0 },
				.right_upper	= { w, 0 },
				.left_lower		= { 0, h },
				.right_lower	= { w, h }
			};
		}

		template <typename FloatPointType = std::pair<float, float>>
		[[nodiscard]] extends<FloatPointType> get_output_size_extends_f() const {
			auto [w, h] = get_output_size_f();
			return {
				.left_upper		= { 0, 0 },
				.right_upper	= { w, 0 },
				.left_lower		= { 0, h },
				.right_lower	= { w, h }
			};
		}

		[[nodiscard]] SDL_BlendMode get_blend_mode() const {
			SDL_BlendMode bm;
			if (SDL_GetRenderDrawBlendMode(m_renderer_ptr, &bm) == -1) {
				// TODO: throw error;
			}
			return bm;
		}

		void set_blend_mode(SDL_BlendMode blend_mode) const {
			if (SDL_SetRenderDrawBlendMode(m_renderer_ptr, blend_mode) == -1) {
				// TODO: throw error;
			}
		}

		void set_render_target(const texture& target) const {
			if (SDL_SetRenderTarget(m_renderer_ptr, target.m_texture_ptr) == -1) {
				// TODO: throw error
			}
		}

		void set_default_render_target() const {
			if (SDL_SetRenderTarget(m_renderer_ptr, nullptr) == -1) {
				// TODO: throw error
			}
		}

		void set_scale(float scale_x, float scale_y) const {
			if (SDL_RenderSetScale(m_renderer_ptr, scale_x, scale_y) == -1) {
				// TODO: throw error
			}
		}

		template<typename PointType = std::pair<float, float>>
		PointType get_scale() const {
			float x;
			float y;
			SDL_RenderGetScale(m_renderer_ptr, &x, &y);

			return { x, y };
		}

		void clear() const {
			SDL_RenderClear(m_renderer_ptr);
		}

		void present() const {
			SDL_RenderPresent(m_renderer_ptr);
		}

		void fill_rect(const SDL_Color& color) const {
			generic_draw(color, [&]() { return SDL_RenderFillRect(m_renderer_ptr, nullptr); });
		}

		void fill_rect(const SDL_Rect& rect, const SDL_Color& color) const {
			generic_draw(color, [&]() { return SDL_RenderFillRect(m_renderer_ptr, &rect); });
		}
		void fill_rect(const SDL_Rect& rect) const {
			generic_draw([&]() { return SDL_RenderFillRect(m_renderer_ptr, &rect); });
		}

		void fill_rect_f(const SDL_FRect& rect, const SDL_Color& color) const {
			generic_draw(color, [&]() { return SDL_RenderFillRectF(m_renderer_ptr, &rect); });
		}
		void fill_rect_f(const SDL_FRect& rect) const {
			generic_draw([&]() { return SDL_RenderFillRectF(m_renderer_ptr, &rect); });
		}

		void draw_rect(const SDL_Rect& rect, const SDL_Color& color) const {
			generic_draw(color, [&]() { return SDL_RenderDrawRect(m_renderer_ptr, &rect); });
		}
		void draw_rect(const SDL_Rect& rect) const {
			generic_draw([&]() { return SDL_RenderDrawRect(m_renderer_ptr, &rect); });
		}

		void draw_rect_f(const SDL_FRect& rect, const SDL_Color& color) const {
			generic_draw(color, [&]() { return SDL_RenderDrawRectF(m_renderer_ptr, &rect); });
		}
		void draw_rect_f(const SDL_FRect& rect) const {
			generic_draw([&]() { return SDL_RenderDrawRectF(m_renderer_ptr, &rect); });
		}

		void draw_line(const SDL_Point& p1, const SDL_Point& p2, const SDL_Color& color) const {
			generic_draw(color, [&]() { return SDL_RenderDrawLine(m_renderer_ptr, p1.x, p1.y, p2.x, p2.y); });
		}
		void draw_line(const SDL_Point& p1, const SDL_Point& p2) const {
			generic_draw([&]() { return SDL_RenderDrawLine(m_renderer_ptr, p1.x, p1.y, p2.x, p2.y); });
		}

		void draw_line_f(const SDL_FPoint& p1, const SDL_FPoint& p2, const SDL_Color& color) const {
			generic_draw(color, [&]() { return SDL_RenderDrawLineF(m_renderer_ptr, p1.x, p1.y, p2.x, p2.y); });
		}
		void draw_line_f(const SDL_FPoint& p1, const SDL_FPoint& p2) const {
			generic_draw([&]() { return SDL_RenderDrawLineF(m_renderer_ptr, p1.x, p1.y, p2.x, p2.y); });
		}

		void draw_lines_f(const SDL_FPoint* points, int count, const SDL_Color& color) const {
			generic_draw(color, [&]() { return SDL_RenderDrawLinesF(m_renderer_ptr, points, count); });
		}

		void draw_point(const SDL_Point& p, const SDL_Color& color) const {
			generic_draw(color, [&]() { return SDL_RenderDrawPoint(m_renderer_ptr, p.x, p.y); });
		}
		void draw_point(const SDL_Point& p) const {
			generic_draw([&]() { return SDL_RenderDrawPoint(m_renderer_ptr, p.x, p.y); });
		}

		void draw_point_f(const SDL_FPoint& p, const SDL_Color& color) const {
			generic_draw(color, [&]() { return SDL_RenderDrawPointF(m_renderer_ptr, p.x, p.y); });
		}
		void draw_point_f(const SDL_FPoint& p) const {
			generic_draw([&]() { return SDL_RenderDrawPointF(m_renderer_ptr, p.x, p.y); });
		}

		template <typename ForwardIt/*, std::enable_if_t<std::is_same_v<std::forward_iterator_tag, typename std::iterator_traits<ForwardIt>::iterator_category>, int> = 0*/>
		void draw_points_f(ForwardIt first, ForwardIt last, const SDL_Color& color) const {
			auto amount = std::distance(first, last);
			generic_draw(color, [&]() { return SDL_RenderDrawPointsF(m_renderer_ptr, &(*first), static_cast<int>(amount)); });
		}

		template <typename ForwardIt /*, std::enable_if_t<std::is_same_v<std::forward_iterator_tag, typename std::iterator_traits<ForwardIt>::iterator_category>, int> = 0*/>
		void draw_points_f(ForwardIt first, int amount, const SDL_Color& color) const {
			generic_draw(color, [&]() { return SDL_RenderDrawPointsF(m_renderer_ptr, &(*first), amount); });
		}

		[[nodiscard]] texture create_texture_from_surface(const surface& surface) const {
			auto ptr = SDL_CreateTextureFromSurface(m_renderer_ptr, surface.m_surface_ptr);

			return texture{ ptr };
		}

		[[nodiscard]] texture create_texture(Uint32 format, int access, int w, int h) const {
			auto ptr = SDL_CreateTexture(m_renderer_ptr, format, access, w, h);

			return texture{ ptr };
		}

		template <typename FloatPoint = SDL_FPoint>
		void copy_f(const texture& texture, const FloatPoint& position) const {
			auto size = texture.get_size();
			SDL_FRect dest{ position.x, position.y, static_cast<float>(size.first), static_cast<float>(size.second) };

			if (SDL_RenderCopyF(m_renderer_ptr, texture.m_texture_ptr, nullptr, &dest) != 0) {
				throw renderer_copy_error();
			}
		}
		void copy(const texture& texture, const SDL_Point& position) const {
			auto size = texture.get_size();
			SDL_Rect dest{ position.x, position.y, size.first, size.second };

			if (SDL_RenderCopy(m_renderer_ptr, texture.m_texture_ptr, nullptr, &dest) == -1) {
				throw renderer_copy_error();
			}
		}

		void copy(const texture& texture) const {
			if (SDL_RenderCopy(m_renderer_ptr, texture.m_texture_ptr, nullptr, nullptr) == -1) {
				throw renderer_copy_error();
			}
		}

		void copy(const texture& texture, const SDL_Rect& destination_rect) const {
			if (SDL_RenderCopy(m_renderer_ptr, texture.m_texture_ptr, nullptr, &destination_rect) == -1) {
				throw renderer_copy_error();
			}
		}

		void copy_f(const texture& texture, const SDL_FRect& destination_rect) const {
			if (SDL_RenderCopyF(m_renderer_ptr, texture.m_texture_ptr, nullptr, &destination_rect) == -1) {
				throw renderer_copy_error();
			}
		}

		void copy(const texture& texture, const SDL_Rect& destination_rect, const SDL_Rect& source_rect) const {
			if (SDL_RenderCopy(m_renderer_ptr, texture.m_texture_ptr, &source_rect, &destination_rect) == -1) {
				throw renderer_copy_error();
			}
		}

		void copy_f(const texture& texture, const SDL_FRect& destination_rect, const SDL_Rect& source_rect) const {
			if (SDL_RenderCopyF(m_renderer_ptr, texture.m_texture_ptr, &source_rect, &destination_rect) == -1) {
				throw renderer_copy_error();
			}
		}

		void copy_ex(const texture& texture, double rotation_angle) const {
			if (SDL_RenderCopyEx(m_renderer_ptr, texture.m_texture_ptr, nullptr, nullptr, rotation_angle, nullptr, SDL_FLIP_NONE) == -1) {
				throw renderer_copy_error();
			}
		}

		void copy_ex(const texture& texture, const SDL_Point& position, double rotation_angle) const {
			auto [w, h] = texture.get_size();
			SDL_Rect dest{ position.x, position.y, w, h };

			if (SDL_RenderCopyEx(m_renderer_ptr, texture.m_texture_ptr, nullptr, &dest, rotation_angle, nullptr, SDL_FLIP_NONE) == -1) {
				throw renderer_copy_error();
			}
		}

		void copy_ex(const texture& texture, const SDL_Rect& destination_rect, double rotation_angle) const {
			if (SDL_RenderCopyEx(m_renderer_ptr, texture.m_texture_ptr, nullptr, &destination_rect, static_cast<double>(rotation_angle), nullptr, SDL_FLIP_NONE) == -1) {
				throw renderer_copy_error();
			}
		}

		void copy_ex_f(const texture& texture, const SDL_FRect& destination_rect, double rotation_angle) const {
			if (SDL_RenderCopyExF(m_renderer_ptr, texture.m_texture_ptr, nullptr, &destination_rect, rotation_angle, nullptr, SDL_FLIP_NONE) == -1) {
				throw renderer_copy_error();
			}
		}

		template<typename PointType>
		void copy_ex_f(const texture& texture, const PointType& position, double rotation_angle) const {
			auto [wi, hi] = texture.get_size();
			auto w = static_cast<float>(wi);
			auto h = static_cast<float>(hi);
			SDL_FRect dest{ position.x - (w / 2.f), position.y - (h / 2.f), w, h };

			if (SDL_RenderCopyExF(m_renderer_ptr, texture.m_texture_ptr, nullptr, &dest, rotation_angle, nullptr, SDL_FLIP_NONE) == -1) {
				throw renderer_copy_error();
			}
		}

		template <typename PointType>
		void copy_ex_f(const texture& texture, const PointType& position, double rotation_angle, const PointType& scale) const {
			auto [wi, hi] = texture.get_size();
			auto w = static_cast<float>(wi) * scale.x;
			auto h = static_cast<float>(hi) * scale.y;
			SDL_FRect dest{ position.x - (w / 2.f), position.y - (h / 2.f), w, h };

			if (SDL_RenderCopyExF(m_renderer_ptr, texture.m_texture_ptr, nullptr, &dest, rotation_angle, nullptr, SDL_FLIP_NONE) == -1) {
				throw renderer_copy_error();
			}
		}

		~renderer() {
			if (m_renderer_ptr != nullptr) {
				SDL_DestroyRenderer(m_renderer_ptr);
			}
		}

	private:

		template <typename Func>
		void generic_draw(const SDL_Color& color, Func func) const {
			auto old_color = get_draw_color();
			set_draw_color(color);

			bool error = func() == -1;
			set_draw_color(old_color);

			if (error) {
				throw renderer_draw_error();
			}
		}

		template <typename Func>
		void generic_draw(Func func) const {
			bool error = func() == -1;

			if (error) {
				throw renderer_draw_error();
			}
		}

		SDL_Renderer* m_renderer_ptr = nullptr;
		friend texture;
	};
}