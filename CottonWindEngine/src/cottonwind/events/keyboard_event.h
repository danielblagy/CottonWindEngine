#pragma once

#include "event.h"


namespace cotwin
{
	class KeyboardKeyEvent : public Event
	{
	public:
		// uses to store SDL_Scancode
		unsigned int keycode;
		const char* keyname;
		bool repeat;

	public:
		KeyboardKeyEvent(unsigned int s_keycode, const char* s_keyname, bool s_repeat)
			: keycode(s_keycode), keyname(s_keyname), repeat(s_repeat)
		{
			category = EventCategoryKeyboard;
		}
	};

	class KeyboardTextInputEvent : public Event
	{
	public:
		const char* text;

	public:
		KeyboardTextInputEvent(char* s_text)
			: text(s_text)
		{
			category = EventCategoryKeyboard;
		}
	};
}