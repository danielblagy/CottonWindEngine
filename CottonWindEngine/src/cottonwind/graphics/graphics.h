#pragma once

#include <SDL.h>
#include <glad/glad.h>

#include "../math.h"
#include "../logger.h"


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
	
	// initialize OpenGL 3.2 Core Profile, returns GLSL version
	static const char* init_opengl()
	{
		// GL 3.2 Core + GLSL 150
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		// TODO : mac support (opengl)
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		// return GLSL version
		return "#version 150";
	}

	// create SDL2 window based on window_properties, handles error checking
	static SDL_Window* create_window(WindowProperties* window_properties)
	{
		if (window_properties->flags & Centered)
		{
			window_properties->left = SDL_WINDOWPOS_CENTERED;
			window_properties->top = SDL_WINDOWPOS_CENTERED;
		}

		Uint32 window_flags = SDL_WINDOW_OPENGL;
		if (window_properties->flags & Fullscreen)
		{
			window_flags = window_flags | SDL_WINDOW_FULLSCREEN;
		}
		else
		{
			if (window_properties->flags & Resizable)
			{
				window_flags = window_flags | SDL_WINDOW_RESIZABLE;
			}

			if (window_properties->flags & Borderless)
			{
				window_flags = window_flags | SDL_WINDOW_BORDERLESS;
			}
		}

		SDL_Window* window = SDL_CreateWindow(
			window_properties->title,
			window_properties->left, window_properties->top,
			window_properties->width, window_properties->height,
			window_flags
		);

		if (window == NULL) {
			LogError("CottonWind\t Could not create window: %s\n", SDL_GetError());
			return false;
		}

		LogTrace("CottonWind\t Initialized SDL2 and SDL_Window");

		return window;
	}

	// create gl context and set it up for rendering into an OpenGL window
	static SDL_GLContext init_opengl_context(SDL_Window* window)
	{
		SDL_GLContext gl_context = SDL_GL_CreateContext(window);
		SDL_GL_MakeCurrent(window, gl_context);

		// Initialize OpenGL loader
		if (gladLoadGL() == 0)
		{
			LogError("CottonWind\t Failed to initialize OpenGL loader!");
			return false;
		}

		LogTrace("CottonWind\t Created GL Context and initialized OpenGL Loader (glad)");

		// display opengl version
		int opengl_major_version, opengl_minor_version;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &opengl_major_version);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &opengl_minor_version);
		LogDebug("CottonWind\t OpenGL version: %d.%d", opengl_major_version, opengl_minor_version);

		return gl_context;
	}

	// NOTE : for now it's a global function, it will probably move to the Renderer class
	static void clear_screen(SDL_Window* window, Vector4f* color)
	{
		int window_w, window_h;
		SDL_GetWindowSize(window, &window_w, &window_h);
		glViewport(0, 0, window_w, window_h);
		glClearColor(color->r, color->g, color->b, color->a);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	// NOTE : for now it's a global function, it will probably move to the Renderer class
	void swap_opengl_buffers(SDL_Window* window)
	{
		SDL_GL_SwapWindow(window);
	}
}