#pragma once
#include <SDL.h>
#include <string_view>
#include <iostream>
#include <array>
#include <memory>
#include <algorithm>
#include <chrono>
#include <entt/entt.hpp>

#include "../sdl/lib.h"
#include "../sdl/lib_ttf.h"
#include "../sdl/renderer.h"
#include "../sdl/font_manager.h"
#include "../sdl/image_manager.h"

//#undef main

namespace sgw {

	struct game_parameters {
		static constexpr double default_time_step = 0.01;
		static constexpr int default_window_x = 10;
		static constexpr int default_window_y = 10;
		static constexpr int default_window_w = 1280;
		static constexpr int default_window_h = 720;
		static constexpr std::string_view default_window_title = "SDL Window";

		Uint32 sdl_lib_flags;
		Uint32 sdl_image_flags;
		std::string_view initial_window_title = default_window_title;
		int window_x = default_window_x;
		int window_y = default_window_y;
		int window_w = default_window_w;
		int window_h = default_window_h;
		Uint32 window_flags;
		Uint32 renderer_flags;
		double game_time_step = default_time_step;
	};

	struct game {

		game() = delete;
		game(game&&) = delete;
		game(const game&) = delete;
		game& operator=(game&&) = delete;
		game& operator=(const game&) = delete;
		~game() = default;

		explicit game(game_parameters params)
			: m_sdl_lib(params.sdl_lib_flags),
			  m_image_manager(params.sdl_image_flags),
			  m_window(params.initial_window_title.data(), params.window_x, params.window_y, params.window_w, params.window_h, params.window_flags),
			  m_renderer(m_window, -1, params.renderer_flags),
			  m_mouse_position(0, 0),
			  m_game_time_step(params.game_time_step) {
		}

		[[nodiscard]] const sdl::lib& get_sdl_lib() const noexcept { return m_sdl_lib; }
		[[nodiscard]] const sdl::lib_ttf& get_sdl_ttf_lib() const noexcept { return m_font_manager.get_lib_ttf(); }
		[[nodiscard]] const sdl::window& get_window() const noexcept { return m_window; }
		[[nodiscard]] const sdl::renderer& get_renderer() const noexcept { return m_renderer; }
		[[nodiscard]] const sgw::font_manager& get_font_manager() const noexcept { return m_font_manager; }
		[[nodiscard]] sgw::font_manager& get_font_manager() noexcept { return m_font_manager; }
		[[nodiscard]] const sgw::image_manager& get_image_manager() const noexcept { return m_image_manager; }
		[[nodiscard]] sgw::image_manager& get_image_manager() noexcept { return m_image_manager; }

		[[nodiscard]] float get_delta_time() const noexcept { return static_cast<float>(m_delta_time); }
		[[nodiscard]] double get_delta_time_precise() const noexcept { return m_delta_time; }
		[[nodiscard]] std::pair<int, int> get_mouse_position() const noexcept { return m_mouse_position; }

		[[nodiscard]] const entt::registry& get_entity_registry() const noexcept { return m_entity_registry; }
		[[nodiscard]] entt::registry& get_entity_registry() noexcept { return m_entity_registry; }

		void signal_quit() noexcept;
		[[nodiscard]] bool is_running() const noexcept { return m_should_run; }

		virtual void start();

	private:
		sdl::lib m_sdl_lib;
		sgw::font_manager m_font_manager;
		sgw::image_manager m_image_manager;
		sdl::window m_window;
		sdl::renderer m_renderer;

		std::pair<int, int> m_mouse_position;

		entt::registry m_entity_registry;
		double m_game_time_step = game_parameters::default_time_step;

		bool m_should_run = true;
		double m_delta_time = 0.0;

		virtual void game_logic() = 0;
		virtual void game_draw(const sdl::renderer& renderer) = 0;
		virtual void handle_event(SDL_Event event) = 0;
		virtual void game_preload() = 0;

		virtual void logic();
		virtual void draw();
		virtual void poll_events();
	};
}