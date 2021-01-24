#pragma once

#include <SDL.h>
#include <glad/glad.h>

#include "../../vendor/glm/glm.hpp"


namespace cotwin
{
	// Whatever graphics implementation, each Renderer class should have clear() and flush() functions
	// that follow the same function declaration

	// OpenGL with SDL2 requires SDL_Window instance to clear/flush
	class Renderer2D
	{
	private:
		static SDL_Window* window;
		// for quick opengl test
		static unsigned int vertex_array;
		static unsigned int vertex_buffer;
		static unsigned int index_buffer;
	
	public:
		static void set_window_instance(SDL_Window* s_window)
		{
			window = s_window;
		}
		
		static void clear(const glm::vec4& color)
		{
			int window_w, window_h;
			SDL_GetWindowSize(window, &window_w, &window_h);
			glViewport(0, 0, window_w, window_h);
			glClearColor(color.r, color.g, color.b, color.a);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		static void flush()
		{
			SDL_GL_SwapWindow(window);
		}

		// for quick opengl test
		static void init_render_data()
		{
			glGenVertexArrays(1, &vertex_array);
			glBindVertexArray(vertex_array);

			glGenBuffers(1, &vertex_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

			float vertices[3 * 3] = {
				-0.5f, -0.5f, 0.0f,
				0.5f, -0.5f, 0.0f,
				0.0f, 0.5f, 0.0f
			};

			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);

			glGenBuffers(1, &index_buffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

			unsigned int indices[3] = {
				0, 1, 2
			};

			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		}

		// for quick opengl test
		static void draw_triangle()
		{
			glBindVertexArray(vertex_array);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
		}
	};

	SDL_Window* Renderer2D::window = 0;

	// for quick opengl test
	unsigned int Renderer2D::vertex_array = 0;
	unsigned int Renderer2D::vertex_buffer = 0;
	unsigned int Renderer2D::index_buffer = 0;
}