#pragma once

#include <SDL.h>

#include "window.h"


namespace cotwin
{
	enum WindowPropertiesFlags
	{
		None = 0,
		Fullscreen = 1 << 0,
		Centered = 1 << 1,
		Resizable = 1 << 2,
		Borderless = 1 << 3,
		Vsync = 1 << 4
	};

	struct WindowProperties
	{
		const char* title;
		int left, top, width, height;
		WindowPropertiesFlags flags;

		WindowProperties(const char* s_title, int s_left, int s_top, int s_width, int s_height, unsigned int s_flags)
			: title(s_title), left(s_left), top(s_top), width(s_width), height(s_height), flags(static_cast<WindowPropertiesFlags>(s_flags))
		{}
	};
	
	class Graphics
	{
	protected:
		SDL_Window* window;

	public:
		virtual bool init(WindowProperties* window_properties) = 0;
		virtual void destroy() = 0;

	protected:
		// to set window handle for Window info utility class
		void set_window(SDL_Window* window)
		{
			Window::set_window(window);
		}
		
		Graphics() {}
		virtual ~Graphics() = default;
	};
}

#ifdef CW_MODERN_OPENGL
#include "opengl/graphics.h"
#include "../imgui/imgui_layer.h"
#else
#include "sdl2/graphics.h"
#endif