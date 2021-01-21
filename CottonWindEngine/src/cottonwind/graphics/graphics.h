#pragma once

#include <SDL.h>

#include "../math.h"


namespace cotwin
{
	enum WindowPropertiesFlags
	{
		None = 0,
		Fullscreen = 1 << 0,
		Centered = 1 << 1,
		Resizable = 1 << 2,
		Borderless = 1 << 3
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

		// TODO : should these be in Renderer ??
		virtual void clear_screen(Vector4f* color) = 0;
		virtual void present() = 0;

	protected:
		Graphics() {}
		virtual ~Graphics() = default;
	};
}