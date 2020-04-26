#pragma once
#include <SDL.h>
#include <tuple>
#include "errors.h"

namespace sdl {
	struct lib {
		constexpr static auto init_timer			= 0x00000001U;
		constexpr static auto init_audio			= 0x00000010U;
		constexpr static auto init_video			= 0x00000020U;
		constexpr static auto init_joystick			= 0x00000200U;
		constexpr static auto init_haptic			= 0x00001000U;
		constexpr static auto init_gamecontroller	= 0x00002000U;
		constexpr static auto init_events			= 0x00004000U;
		constexpr static auto init_sensor			= 0x00008000U;
		constexpr static auto init_everything = 
			init_timer | init_audio | init_video | init_joystick | init_haptic | init_gamecontroller | init_events | init_sensor;

		lib() = delete;
		lib(const lib&) = delete;
		lib(lib&&) = delete;
		lib& operator=(const lib&) = delete;
		lib& operator=(lib&&) = delete;

		explicit lib(Uint32 flags) {
			if (SDL_Init(flags) != 0) {
				throw init_error();
			}
		}

		[[nodiscard]] static bool subsystem_was_init(Uint32 flags) {
			return SDL_WasInit(flags) == flags;
		}

		[[nodiscard]] static bool set_hint(std::string_view name, std::string_view value) {
			return SDL_SetHint(name.data(), value.data()) == SDL_TRUE;
		}

		[[nodiscard]] static std::string_view get_hint(std::string_view name) {
			return SDL_GetHint(name.data());
		}

		[[nodiscard]] static bool get_hint_bool(std::string_view name/*, bool default_value*/) {
			return SDL_GetHintBoolean(name.data(), SDL_TRUE) == SDL_TRUE;
		}

		template <typename Type = std::tuple<float, float, float>>
		[[nodiscard]] static Type get_display_dpi(int display_index) {
			float ddpi;
			float hdpi;
			float vdpi;
			SDL_GetDisplayDPI(display_index, &ddpi, &hdpi, &vdpi);

			return { ddpi, hdpi, vdpi };
		}

		[[nodiscard]] static int get_number_of_render_drivers() {
			return SDL_GetNumRenderDrivers();
		}

		[[nodiscard]] static SDL_RendererInfo get_render_driver_info(int index) {
			SDL_RendererInfo info;
			SDL_GetRenderDriverInfo(index, &info);

			return info;
		}

		[[nodiscard]] static const char * get_video_driver(int index) {
			return SDL_GetVideoDriver(index);
		}

		~lib() {
			SDL_Quit();
		}
	};
}