#pragma once

#include <SDL.h>


namespace cotwin
{
	class KeyboardInput
	{
	public:
		static void is_key_pressed(unsigned int keycode)
		{
			const Uint8 *state = SDL_GetKeyboardState(NULL);

		}
	};
}