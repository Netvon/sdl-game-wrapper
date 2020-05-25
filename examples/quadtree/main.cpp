#include <sgw/sgw.h>
#include <sgw/util/quadtree.h>

struct game : sgw::game {

	explicit game(sgw::game_parameters params) : sgw::game(params), m_quadtree(glm::vec4(200.F, 200.F, 400.F, 400.F)) { }


	void game_logic() override
	{

	}

	void game_draw(const sdl::renderer& renderer) override
	{
		m_quadtree.for_each_boundary([&](const glm::vec4& boundary) {
			renderer.draw_rect_f(
				SDL_FRect{ boundary[0] - boundary[2], boundary[1] - boundary[3], boundary[2] * 2.F, boundary[3] * 2.F}, 
				SDL_Color{ 255, 255, 255, 255 });
		});

		m_quadtree.for_each([&](const glm::vec2& point) {
			renderer.fill_rect_f(
				SDL_FRect{ point[0] - 5.F, point[1] - 5.F, 10.F, 10.F },
				SDL_Color{ 255, 0, 0, 255 });
		});

	}

	void handle_event(SDL_Event event) override
	{

	}

	void game_preload() override
	{
		for (std::size_t i = 0; i < 100; i++)
		{
			m_quadtree.insert(glm::vec2(sgw::random::next(0.F, 400.F), sgw::random::next(0.F, 400.F)));
		}
	}

private:
	sgw::util::quadtree m_quadtree;
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {

	constexpr sgw::game_parameters params{
		.sdl_lib_flags = sdl::lib::init_everything,
		.sdl_image_flags = IMG_INIT_PNG,
		.initial_window_title = "quadtree",
		.window_x = sdl::window::position_centered,
		.window_y = sdl::window::position_centered,
		.window_w = 1280,
		.window_h = 720,
		.window_flags = 0,
		.renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	};

	game g(params);
	g.start();

	return 0;
}