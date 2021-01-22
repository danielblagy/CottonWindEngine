#pragma once

#include "../graphics.h"
#include <glad/glad.h>

#include "../../logger.h"


namespace cotwin
{
	class OpenGLGraphics : public Graphics
	{
	private:
		SDL_GLContext gl_context;
		const char* glsl_version;

	public:
		bool init(WindowProperties* window_properties) override
		{
			// initialize SDL & OpenGL

			int sdl_init_result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

			if (sdl_init_result != 0)
			{
				LogCritical("CottonWind\t Unable to initialize SDL: %s", SDL_GetError());
				return false;
			}

			glsl_version = "#version 150";

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

			// create a window

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

			window = SDL_CreateWindow(
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

			// create gl context and set it up for rendering into an OpenGL window
			
			gl_context = SDL_GL_CreateContext(window);
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

			return true;
		}

		void destroy() override
		{
			LogTrace("CottonWind\t OpenGL & SLD2 Cleanup");

			SDL_GL_DeleteContext(gl_context);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}

		void enable_vsync(double* delta_cap) override
		{
			*delta_cap = 0.0;
			SDL_GL_SetSwapInterval(1);
		}

		void disable_vsync(double* delta_cap) override
		{
			*delta_cap = 0.0;
			SDL_GL_SetSwapInterval(0);
		}

		inline SDL_Window* get_window() { return window; }
		inline SDL_GLContext get_gl_context() { return gl_context; }
		inline const char* get_glsl_version() { return glsl_version; }
	};

	// NOTE : just a test of opengl
	void init_render_data(unsigned int* vertex_array, unsigned int* vertex_buffer, unsigned int* index_buffer)
	{
		glGenVertexArrays(1, vertex_array);
		glBindVertexArray(*vertex_array);

		glGenBuffers(1, vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer);

		float vertices[3 * 3] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);

		glGenBuffers(1, index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *index_buffer);

		unsigned int indices[3] = {
			0, 1, 2
		};

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	}

	void draw_triangle(unsigned int* vertex_array)
	{
		glBindVertexArray(*vertex_array);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
	}
}