#define SGW_USE_OPENGL

#include <sgw/sgw.h>
#include <sgw/gl/program.h>
#include <sgw/gl/mesh.h>
#include <sgw/gl/texture.h>
#include <sgw/gl/buffer.h>
#include <sgw/gl/renderer.h>
#include <sgw/gl/frame_buffer.h>
#include <sgw/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <sgw/util/quadtree.h>

#include <array>
#include <filesystem>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"


constexpr bool enable_imgui = true;

struct sprite_data {

	// [0] = x; [1] = y; [2] = z
	glm::vec4 translate{ 0.F, 0.F, 0.F, 0.F };

	// [0] = scale x; [1] = scale y; [2] = rotation
	glm::vec4 scale_rotation{ 0.F, 0.F, 0.F, 0.F };

	// [0] = r; [1] = g; [2] = b; [3] = a
	glm::vec4 tint{ 0.F, 0.F, 0.F, 0.F };

	std::array<glm::vec2, 4> uvs{
		glm::vec2{0.F, 0.F},
		glm::vec2{0.F, 0.F},
		glm::vec2{0.F, 0.F},
		glm::vec2{0.F, 0.F}
	};
};

glm::vec4 worldspace_to_viewspace(glm::mat4 projection, glm::mat4 view, glm::vec4 point) {
	return projection * view * point;
}

struct demo_params {
	std::size_t sprite_amount = 100;
};

struct game : sgw::game {

	using program = sgw::program;
	using shader = sgw::shader;
	//using mesh = sgw::mesh;
	//using vertex_attribute = sgw::vertex_attribute;
	using texture = sgw::texture2d;
	using ssbo = sgw::shader_storage_buffer;

	explicit game(sgw::game_parameters params, demo_params demo)
		: sgw::game(params), m_sprite_amount(demo.sprite_amount) { }

	void game_logic() override
	{
		//constexpr auto s = sizeof(sprite_data);
		//constexpr auto s1 = sizeof(float) * 19;

		//m_rotation += get_delta_time() * 100.F;
	}

	void game_draw() override
	{
		if (m_mesh_changed) {
			glActiveTexture(GL_TEXTURE0);
			//m_texture.bind();
			//m_prog.use();
			m_quad_mesh.bind();
			m_sprite_data.bind();

			sgw::bind_buffer(m_sprite_data, 0);

			m_screen_prog.use();
			m_screen_prog.set("texture1", 0);

			m_prog.use();
			m_prog.set("texture1", 0);
			

			m_mesh_changed = false;
		}

		if (m_view_changed) {
			try
			{
				m_prog.use();
				m_prog.set("projection", m_projection);
				m_prog.set("view", m_view);

				m_view_changed = false;
			}
			catch (const sgw::unknown_uniform_error& ex)
			{
				std::cerr << "SHADER ERROR, UNKNOWN UNIFORM: " << ex.what() << '\n';
			}
			catch (const std::exception& ex)
			{
				std::cerr << "UNKNOWN ERROR: " << ex.what() << '\n';
			}
		}

		use_sprite();

		m_frame_buffer.bind();

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1F, 0.25F, 0.5F, 1.0F);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_quad_mesh.draw_instanced(m_sprite_amount, sgw::mesh::draw_operation_buffer_handling::manual_bind_and_unbind);

		m_frame_buffer.unbind();

		use_screen_quad();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glClearColor(1.0F, 1.0F, 1.0F, 1.0F);
		glClear(GL_COLOR_BUFFER_BIT);

		m_quad_mesh.draw_instanced(1, sgw::mesh::draw_operation_buffer_handling::manual_bind_and_unbind);
		imgui_draw();
	}

	void use_sprite() const {
		m_texture.bind();
		m_prog.use();
	}

	void use_screen_quad() const {
		m_frame_buffer.get_texture().bind();
		m_screen_prog.use();
	}

	void imgui_draw()
	{
		if constexpr (enable_imgui) {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(get_window().get_ptr());
			ImGui::NewFrame();

			ImGui::ShowDemoWindow(&m_show_demo);

			show_sprite_editor();
			//show_body_editor();
			show_view_editor();
			//show_projection_editor();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
	}

	void show_view_editor()
	{
		static bool show_view_editor = true;
		ImGui::Begin("View Editor", &show_view_editor);

		float scale = m_view[0][0];

		ImGui::Text("Scale");

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

		if (ImGui::SliderFloat("##scalex", &scale, 0.001F, 10.F, "%.4F", 10.F)) {
			m_view[0][0] = scale;
			m_view[1][1] = scale;

			m_view_changed = true;
		}

		ImGui::PopItemWidth();

		ImGui::Text("Offset");

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 2.F);
		if (ImGui::DragFloat("##X", &m_view[3][0], 1.F, 0.0F, 0.0F, "X = %.3F")) { m_view_changed = true; }
		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &m_view[3][1], 1.F, 0.0F, 0.0F, "Y = %.3F")) { m_view_changed = true; }
		ImGui::PopItemWidth();

		if (ImGui::CollapsingHeader("Raw")) {
			static float drag_speed = 0.1F;
			draw_matrix(m_view, drag_speed);
		}

		ImGui::End();
	}

	void show_projection_editor()
	{
		static bool show_view_editor = true;
		static float drag_speed = 0.1F;
		ImGui::Begin("Projection Editor", &show_view_editor);

		draw_matrix(m_projection, drag_speed);
		ImGui::End();
	}

	void show_sprite_editor() {
		static bool show_me = true;

		if (show_me) {
			ImGui::Begin("Sprite Editor", &show_me);

			static int index = 0;
			ImGui::Text("Sprite");
			ImGui::SliderInt("Index", &index, 0, m_sprite_amount - 1); ImGui::SameLine();
			ImGui::PushButtonRepeat(true);
			if (ImGui::ArrowButton("idx_up", ImGuiDir_Up)) {
				index++;
				index %= m_sprite_amount;
			}
			ImGui::SameLine();
			if (ImGui::ArrowButton("idx_down", ImGuiDir_Down)) {
				index--;
				if (index < 0) {
					index = m_sprite_amount - 1;
				}
			}
			ImGui::PopButtonRepeat();
			ImGui::Separator();

			ImGui::BeginChild("sprite_data");

			sprite_data d;
			m_sprite_data.get_data(d, index);

			bool changed = false;

			if (ImGui::DragFloat3("translate", &d.translate[0])) { changed = true; }
			if (ImGui::DragFloat2("scale", &d.scale_rotation[0])) { changed = true; }
			if (ImGui::DragFloat("rotation", &d.scale_rotation[2])) { changed = true; }

			if (ImGui::ColorEdit3("tint", &d.tint[0])) { changed = true; }

			if (ImGui::CollapsingHeader("UVs")) {
				//ImGui::Text("UVs");
				if (ImGui::DragFloat2("top right", &d.uvs[0][0], 0.01F, 0.0F, 1.0F)) { changed = true; }
				if (ImGui::DragFloat2("bottom right", &d.uvs[1][0], 0.01F, 0.0F, 1.0F)) { changed = true; }
				if (ImGui::DragFloat2("bottom left", &d.uvs[2][0], 0.01F, 0.0F, 1.0F)) { changed = true; }
				if (ImGui::DragFloat2("top left", &d.uvs[3][0], 0.01F, 0.0F, 1.0F)) { changed = true; }
			}


			if (changed) {
				m_sprite_data.set_data(d, index);
			}

			ImGui::EndChild();
			ImGui::End();
		}
	}

	void show_body_editor()
	{
		//if (m_show_another_window) {
		//	ImGui::Begin("Body Editor", &m_show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

		//	static int index = 1000 - 6;
		//	ImGui::Text("Body");
		//	ImGui::SliderInt("Index", &index, 0, 1000 - 1); ImGui::SameLine();
		//	ImGui::PushButtonRepeat(true);
		//	if (ImGui::ArrowButton("idx_up", ImGuiDir_Up)) {
		//		index++;
		//		index %= 1000;
		//	}
		//	ImGui::SameLine();
		//	if (ImGui::ArrowButton("idx_down", ImGuiDir_Down)) {
		//		index--;
		//		if (index < 0) {
		//			index = 999;
		//		}
		//	}
		//	ImGui::PopButtonRepeat();
		//	ImGui::Separator();
		//	ImGui::BeginChild("body_data");

		//	auto& ssbo = m_mesh.get_ssbo();

		//	glm::mat4 test_subject(0.F);
		//	glm::vec4 color(0.F);
		//	glm::vec4 physics(0.F);
		//	ssbo.get_data(test_subject, index);
		//	m_colors.get_data(color, index);
		//	m_physics.get_data(physics, index);

		//	bool changed = false;

		//	if (draw_offset(test_subject, "Position")) {
		//		changed = true;
		//	}

		//	if (draw_offset(test_subject, "Scale", 0, 0, 1, 1)) {
		//		changed = true;
		//	}

		//	if (ImGui::CollapsingHeader("Raw")) {
		//		static float drag_speed = 0.1F;
		//		if (draw_matrix(test_subject, drag_speed)) {
		//			changed = true;
		//		}
		//	}

		//	if (changed) {
		//		//glm::mat4 t = make_model(glm::vec3(0.F), 1.F);
		//		//ssbo.set_data(t, index);
		//		ssbo.set_data(test_subject, index);
		//	}

		//	ImGui::Separator();
		//	ImGui::Text("Color");
		//	if (ImGui::ColorEdit3("##color", &color[0])) { m_colors.set_data(color, index); }

		//	//ImGui::Text("Physics");
		//	//ImGui::PushItemWidth(80);
		//	//if (ImGui::SliderFloat("vx", &physics[0], -1000.F, 1000.F)) { changed = true; } ImGui::SameLine();
		//	//if (ImGui::SliderFloat("vy", &physics[1], -1000.F, 1000.F)) { changed = true; } ImGui::SameLine();
		//	//if (ImGui::SliderFloat("mass", &physics[2], 0.F, 1.F)) { changed = true; }
		//	//ImGui::PopItemWidth();

		//	//if (ImGui::Button("Close Me"))
		//	//	m_show_another_window = false;
		//	//ImGui::PopItemWidth();

		//	ImGui::EndChild();
		//	ImGui::End();
		//}
	}

	bool draw_offset(glm::mat4& test_subject, const char* header = "Offset",
		std::size_t value_a_row = 0, std::size_t value_a_column = 3,
		std::size_t value_b_row = 1, std::size_t value_b_column = 3)
	{
		bool changed = false;
		ImGui::Text(header);

		std::string str_x = "##x";
		str_x = str_x.append("_");
		str_x = str_x.append(header);
		str_x = str_x.append(2, value_a_row + 'a');
		str_x = str_x.append(2, value_b_row + 'a');
		str_x = str_x.append(2, value_a_column + 'a');
		str_x = str_x.append(2, value_b_column + 'a');

		std::string str_y = "##y";
		str_y = str_y.append("_");
		str_y = str_y.append(header);
		str_y = str_y.append(2, value_a_row + 'a');
		str_y = str_y.append(2, value_b_row + 'a');
		str_y = str_y.append(2, value_a_column + 'a');
		str_y = str_y.append(2, value_b_column + 'a');

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 2.F);
		if (ImGui::DragFloat(str_x.c_str(), &test_subject[value_a_column][value_a_row], 1.F, 0.0F, 0.0F, "X = %.3F")) {
			changed = true;
		}
		ImGui::SameLine();
		if (ImGui::DragFloat(str_y.c_str(), &test_subject[value_b_column][value_b_row], 1.F, 0.0F, 0.0F, "Y = %.3F")) {
			changed = true;
		}
		ImGui::PopItemWidth();

		return changed;
	}

	bool draw_matrix(glm::mat4& matrix, float& drag_speed) {
		bool changed = false;

		auto r1 = std::array{ matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0] };
		auto r2 = std::array{ matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1] };
		auto r3 = std::array{ matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2] };
		auto r4 = std::array{ matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3] };

		ImGui::DragFloat("Drag Speed", &drag_speed, 0.1F, 0.001F, 10.F);

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat4("##r1", r1.data(), drag_speed)) { changed = true; matrix[0][0] = r1[0]; matrix[1][0] = r1[1]; matrix[2][0] = r1[2]; matrix[3][0] = r1[3]; }
		if (ImGui::DragFloat4("##r2", r2.data(), drag_speed)) { changed = true; matrix[0][1] = r2[0]; matrix[1][1] = r2[1]; matrix[2][1] = r2[2]; matrix[3][1] = r2[3]; }
		if (ImGui::DragFloat4("##r3", r3.data(), drag_speed)) { changed = true; matrix[0][2] = r3[0]; matrix[1][2] = r3[1]; matrix[2][2] = r3[2]; matrix[3][2] = r3[3]; }
		if (ImGui::DragFloat4("##r4", r4.data(), drag_speed)) { changed = true; matrix[0][3] = r4[0]; matrix[1][3] = r4[1]; matrix[2][3] = r4[2]; matrix[3][3] = r4[3]; }

		return changed;
	}

	void handle_event(SDL_Event event) override
	{
		bool mouse_free = true;
		if constexpr (enable_imgui) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			ImGuiIO& io = ImGui::GetIO();

			mouse_free = !io.WantCaptureMouse;
		}

		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
			update_projection_and_view();
		}

		if (mouse_free) {
			if (event.type == SDL_MOUSEMOTION && event.motion.state & SDL_BUTTON_LMASK) {
				m_view[3][0] += static_cast<float>(event.motion.xrel);
				m_view[3][1] += static_cast<float>(event.motion.yrel);

				m_view_changed = true;
			}

			if (event.type == SDL_MOUSEWHEEL) {
				glm::vec2 scale(m_view[0][0], m_view[1][1]);
				scale += glm::vec2(static_cast<float>(event.wheel.y), static_cast<float>(event.wheel.y)) * (scale * 0.1F);

				m_view[0][0] = scale[0];
				m_view[1][1] = scale[1];

				m_view_changed = true;
			}
		}
	}

	glm::mat4 make_model(glm::vec3 offset, float scale = 1.0f) {
		auto [w, h] = get_window().get_window_size<std::pair<float, float>>();
		auto model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(w * 0.5F, h * 0.5F, 1.0f));
		model = glm::translate(model, offset);

		model = glm::rotate(model, glm::radians(m_rotation), glm::vec3(0.F, 0.F, 1.F));
		model = glm::scale(model, glm::vec3(glm::vec2(19.F, 29.F) * scale, 0.F));

		return model;
	}

	void update_projection_and_view()
	{
		auto [w, h] = get_window().get_window_size<std::pair<float, float>>();
		m_projection = glm::ortho(0.F, w, h, 0.F, -128.F, 128.F);

		m_view_changed = true;
	}

	void game_preload() override
	{
		imgui_setup();

		try
		{
			m_prog = program::create_and_link(
				shader::create_and_compile_from_file(shader::vertex_type, "assets/shaders/sprite_shader_instanced_new.vertex.glsl"),
				shader::create_and_compile_from_file(shader::fragment_type, "assets/shaders/sprite_shader.fragment.glsl")
			);

			m_screen_prog = program::create_and_link(
				shader::create_and_compile_from_file(shader::vertex_type, "assets/shaders/screen_texture.vertex.glsl"),
				shader::create_and_compile_from_file(shader::fragment_type, "assets/shaders/screen_texture.fragment.glsl")
			);

			std::vector<sprite_data> sprite_data_list;

			auto [w, h] = get_window().get_window_size<std::pair<float, float>>();
			auto offset = glm::vec4(w * 0.5F, h * 0.5F, 0.F, 0.F);

			{
				auto half_amount = static_cast<float>(m_sprite_amount) * 0.1F;
				for (std::size_t i = 0; i < m_sprite_amount; i++)
				{
					sprite_data sd;
					sd.translate = glm::vec4(
						sgw::random::next(-half_amount, half_amount), sgw::random::next(-half_amount, half_amount),
						0.F, 0.F) + offset;

					sd.scale_rotation = glm::vec4(19.F, 29.F, 0.F, 0.F);
					//sd.rotation = sgw::random::next(0.F, 180.F);
					sd.tint = glm::vec4(1.F, 1.F, 1.F, 1.F);
					sd.uvs[0] = glm::vec2{ 0.5F, 1.0F };
					sd.uvs[1] = glm::vec2{ 0.5F, 0.0F };
					sd.uvs[2] = glm::vec2{ 0.0F, 0.0F };
					sd.uvs[3] = glm::vec2{ 0.0F, 1.0F };

					auto pick = static_cast<bool>(sgw::random::next(0, 1));

					if (pick) {
						sd.uvs[0] = glm::vec2{ 1.0F, 1.0F };
						sd.uvs[1] = glm::vec2{ 1.0F, 0.0F };
						sd.uvs[2] = glm::vec2{ 0.5F, 0.0F };
						sd.uvs[3] = glm::vec2{ 0.5F, 1.0F };
					}

					sprite_data_list.push_back(sd);
				}
			}

			m_sprite_data = ssbo::make_buffer_from_container(sprite_data_list, ssbo::usage::dynamic_copy);

			m_quad_mesh = sgw::primitives::make_quad(sgw::mesh::after_create_operation::nothing);

			m_texture = texture::create_from_file(
				"assets/textures/enemy_map.png",
				sgw::texture_filtering::nearest,
				sgw::texture_mipmap::enable);

			//m_prog.use();
			//m_prog.set("texture1", 0);
			update_projection_and_view();

			m_frame_buffer = sgw::frame_buffer::make(1280, 720);

			auto status = m_frame_buffer.get_status();

			if (!m_frame_buffer.is_valid()) {
				throw std::runtime_error("Framebuffer incomplete");
			}

			std::cout << "Mesh and Program created\n";

			//sgw::bind_buffer(m_sprite_data, 0);
		}
		catch (const sgw::shader_compile_error& ex)
		{
			std::cerr << "SHADER COMPILE ERROR: " << ex.what() << '\n';
		}
		catch (const sgw::unknown_uniform_error& ex)
		{
			std::cerr << "SHADER ERROR, UNKNOWN UNIFORM: " << ex.what() << '\n';
		}
		catch (const std::exception& ex)
		{
			std::cerr << ex.what();
		}
	}

	void imgui_setup()
	{
		if constexpr (enable_imgui) {
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;

			ImGui::StyleColorsDark();

			ImGui_ImplSDL2_InitForOpenGL(
				get_window().get_ptr(),
				get_gl_context().get_ptr());

			ImGui_ImplOpenGL3_Init("#version 130");
		}
	}

	program m_prog;
	program m_screen_prog;
	sgw::mesh m_quad_mesh;
	texture m_texture;
	sgw::frame_buffer m_frame_buffer;

	glm::mat4 m_model;
	glm::mat4 m_view = glm::mat4(1.F);
	glm::mat4 m_projection;

	ssbo m_sprite_data;

	float m_rotation = 1.F;
	bool m_show_another_window = true;
	bool m_show_demo = true;

	bool m_mesh_changed = true;
	bool m_view_changed = true;

	std::size_t m_sprite_amount = 1000;
};


int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
	constexpr sgw::game_parameters params{
		.sdl_lib_flags = sdl::lib::init_everything,
		.initial_window_title = "opengl example",
		.window_x = sdl::window::position_centered,
		.window_y = sdl::window::position_centered,
		.window_w = 1280,
		.window_h = 720,
		.window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE,
		.gl_auto_clear = false
	};

	constexpr demo_params dp{
		.sprite_amount = static_cast<std::size_t>(1E5)
	};

	game g(params, dp);
	g.start();

	return 0;
}