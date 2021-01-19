#pragma once

#include <SDL.h>

#include "../math.h"


namespace cotwin
{
	class Input
	{
	public:
		static bool is_key_pressed(unsigned int keycode)
		{
			// TODO : is SDL_PumpEvents needed here ??
			SDL_PumpEvents();
			const Uint8 *state = SDL_GetKeyboardState(NULL);
			return state[keycode];
		}

		static bool is_key_pressed(const char* keyname)
		{
			SDL_PumpEvents();
			const Uint8 *state = SDL_GetKeyboardState(NULL);
			return state[SDL_GetScancodeFromName(keyname)];
		}

		static bool is_mouse_button_pressed(unsigned int button)
		{
			SDL_PumpEvents();
			return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(button);
		}

		static Vector2 get_mouse_position()
		{
			Vector2 mouse_position;
			SDL_PumpEvents();
			SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
			return mouse_position;
		}

		static bool is_key_mod_pressed(unsigned int keymod)
		{
			return (SDL_GetModState() & keymod) != 0;
		}
	};
}