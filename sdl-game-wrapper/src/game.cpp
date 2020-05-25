//#include "../include/game/game.h"
//
//namespace sgw {
//	void game::signal_quit() noexcept { m_should_run = false; }
//
//	void game::start() {
//		try {
//			game_preload();
//
//			m_delta_time = m_game_time_step;
//			auto time_start = std::chrono::system_clock::now();
//			auto accumulator = 0.0;
//
//			while (m_should_run) {
//				auto time_now = std::chrono::system_clock::now();
//				std::chrono::duration<double> frame_time = time_now - time_start;
//				auto frame_time_val = frame_time.count();
//
//				if (frame_time_val > 0.25) {
//					frame_time_val = 0.25;
//				}
//
//				time_start = time_now;
//				accumulator += frame_time_val;
//
//				while (accumulator >= m_delta_time) {
//					logic();
//					accumulator -= m_delta_time;
//				}
//
//				poll_events();
//				draw();
//			}
//		}
//		catch (const std::exception& ex) {
//			std::cout << ex.what();
//		}
//	}
//
//	void game::logic() {
//		game_logic();
//	}
//	
//	void game::draw() {
//		m_renderer.clear();
//		game_draw(m_renderer);
//		m_renderer.present();
//	}
//	
//	void game::poll_events() {
//		SDL_Event sdl_event;
//
//		if (SDL_PollEvent(&sdl_event) == 1) {
//
//			if (sdl_event.type == SDL_QUIT) {
//				m_should_run = false;
//			}
//			else if (sdl_event.type == SDL_MOUSEMOTION) {
//				m_mouse_position.first = sdl_event.motion.x;
//				m_mouse_position.second = sdl_event.motion.y;
//			}
//
//			handle_event(sdl_event);
//		}
//	}
//}