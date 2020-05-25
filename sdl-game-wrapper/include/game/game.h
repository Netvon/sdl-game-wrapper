#ifndef SGW_GAME_GAME_H
#define SGW_GAME_GAME_H
#include "../sdl/lib.h"
#include "../sdl/window.h"

#include <SDL.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <string_view>

//#undef main
//#define SGW_USE_OPENGL
#ifndef SGW_USE_OPENGL
#include "../sdl/font_manager.h"
#include "../sdl/image_manager.h"
#include "../sdl/lib_ttf.h"
#include "../sdl/renderer.h"
#else
#include "../gl/context.h"
#endif // !SGW_USE_OPENGL

namespace sgw {

    struct game_parameters {
        static constexpr double default_time_step              = 0.01;
        static constexpr int default_window_x                  = 10;
        static constexpr int default_window_y                  = 10;
        static constexpr int default_window_w                  = 1280;
        static constexpr int default_window_h                  = 720;
        static constexpr std::string_view default_window_title = "SDL Window";

        Uint32 sdl_lib_flags;
#ifndef SGW_USE_OPENGL
        Uint32 sdl_image_flags;
#endif // !SGW_USE_OPENGL
        std::string_view initial_window_title = default_window_title;
        int window_x                          = default_window_x;
        int window_y                          = default_window_y;
        int window_w                          = default_window_w;
        int window_h                          = default_window_h;
#ifdef SGW_USE_OPENGL
        Uint32 window_flags = SDL_WINDOW_OPENGL;
#else
        Uint32 window_flags;
        Uint32 renderer_flags;
#endif // SGW_USE_OPENGL
        double game_time_step = default_time_step;
    };

    struct game {

        game()            = delete;
        game(game&&)      = delete;
        game(const game&) = delete;
        game& operator=(game&&) = delete;
        game& operator=(const game&) = delete;
        ~game()                      = default;

        explicit game(game_parameters params)
            : m_sdl_lib(params.sdl_lib_flags)
            ,
#ifndef SGW_USE_OPENGL
            m_image_manager(params.sdl_image_flags)
            ,
#endif // !SGW_USE_OPENGL
            m_window(params.initial_window_title.data(), params.window_x, params.window_y, params.window_w, params.window_h, params.window_flags)
            ,
#ifdef SGW_USE_OPENGL
            m_gl_context(m_window)
            ,
#else
            m_renderer(m_window, -1, params.renderer_flags)
            ,
#endif
            m_mouse_position(0, 0)
            , m_game_time_step(params.game_time_step) {
        }

#ifdef SGW_USE_OPENGL
        [[nodiscard]] const sgw::context& get_gl_context() noexcept { return m_gl_context; }
#else
        [[nodiscard]] const sdl::lib_ttf& get_sdl_ttf_lib() const noexcept { return m_font_manager.get_lib_ttf(); }
        [[nodiscard]] const sdl::renderer& get_renderer() const noexcept { return m_renderer; }
        [[nodiscard]] const sgw::font_manager& get_font_manager() const noexcept { return m_font_manager; }
        [[nodiscard]] sgw::font_manager& get_font_manager() noexcept { return m_font_manager; }
        [[nodiscard]] const sgw::image_manager& get_image_manager() const noexcept { return m_image_manager; }
        [[nodiscard]] sgw::image_manager& get_image_manager() noexcept { return m_image_manager; }
#endif // !SGW_USE_OPENGL

        [[nodiscard]] const sdl::lib& get_sdl_lib() const noexcept { return m_sdl_lib; }
        [[nodiscard]] const sdl::window& get_window() const noexcept { return m_window; }

        [[nodiscard]] float get_delta_time() const noexcept { return static_cast<float>(m_delta_time); }
        [[nodiscard]] double get_delta_time_precise() const noexcept { return m_delta_time; }
        [[nodiscard]] std::pair<int, int> get_mouse_position() const noexcept { return m_mouse_position; }

        [[nodiscard]] const entt::registry& get_entity_registry() const noexcept { return m_entity_registry; }
        [[nodiscard]] entt::registry& get_entity_registry() noexcept { return m_entity_registry; }

        void signal_quit() noexcept { m_should_run = false; }
        [[nodiscard]] bool is_running() const noexcept { return m_should_run; }

        virtual void start() {
            try {
                preload();

                m_delta_time     = m_game_time_step;
                auto time_start  = std::chrono::system_clock::now();
                auto accumulator = 0.0;

                while (m_should_run) {
                    auto time_now                            = std::chrono::system_clock::now();
                    std::chrono::duration<double> frame_time = time_now - time_start;
                    auto frame_time_val                      = frame_time.count();

                    if (frame_time_val > 0.25) { frame_time_val = 0.25; }

                    time_start = time_now;
                    accumulator += frame_time_val;

                    while (accumulator >= m_delta_time) {
                        logic();
                        accumulator -= m_delta_time;
                    }

                    poll_events();
                    draw();
                }
            }
            catch (const std::exception& ex) {
                std::cout << ex.what();
            }
        }

    private:
        sdl::lib m_sdl_lib;
#ifndef SGW_USE_OPENGL
        sgw::font_manager m_font_manager;
        sgw::image_manager m_image_manager;
#endif // !SGW_USE_OPENGL
        sdl::window m_window;
#ifndef SGW_USE_OPENGL
        sdl::renderer m_renderer;
#else
        sgw::context m_gl_context;
#endif // !SGW_USE_OPENGL

        std::pair<int, int> m_mouse_position;

        entt::registry m_entity_registry;
        double m_game_time_step = game_parameters::default_time_step;

        bool m_should_run   = true;
        double m_delta_time = 0.0;
#ifdef SGW_USE_OPENGL
        glm::vec4 m_clear_color{ 0.1F, 0.1F, 0.1F, 1.F };
#endif // SGW_USE_OPENGL

        virtual void game_logic() = 0;
#ifdef SGW_USE_OPENGL
        virtual void game_draw() = 0;
#else
        virtual void game_draw(const sdl::renderer& renderer) = 0;
#endif // SGW_USE_OPENGL
        virtual void handle_event(SDL_Event event) = 0;
        virtual void game_preload()                = 0;

        virtual void logic() { game_logic(); }

        virtual void draw() {
#ifdef SGW_USE_OPENGL
            glClearColor(m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            game_draw();
            m_window.gl_swap_window();
#else
            m_renderer.clear();
            game_draw(m_renderer);
            m_renderer.present();
#endif // SGW_USE_OPENGL
        }

        virtual void poll_events() {
            SDL_Event sdl_event;

            if (SDL_PollEvent(&sdl_event) == 1) {

                if (sdl_event.type == SDL_QUIT) { m_should_run = false; }
                else if (sdl_event.type == SDL_MOUSEMOTION) {
                    m_mouse_position.first  = sdl_event.motion.x;
                    m_mouse_position.second = sdl_event.motion.y;
                }
#ifdef SGW_USE_OPENGL
                else if (sdl_event.type == SDL_WINDOWEVENT && sdl_event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    glViewport(0, 0, sdl_event.window.data1, sdl_event.window.data2);
                }
#endif

                handle_event(sdl_event);
            }
        }

        virtual void preload() {
#ifdef SGW_USE_OPENGL
            auto [w, h] = m_window.get_window_size();
            glViewport(0, 0, w, h);
#endif // SGW_USE_OPENGL
            game_preload();
        }
    };
}
#endif // SGW_GAME_GAME_H