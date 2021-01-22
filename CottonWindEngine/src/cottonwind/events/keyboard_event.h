#pragma once

#include "event.h"


namespace cotwin
{
	/*class KeyboardKeyEvent : public Event
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
	};*/

	class KeyboardEvent : public Event
	{
	public:
		union
		{
			struct
			{
				unsigned int keycode; // uses to store SDL_Scancode
				const char* keyname;
				bool repeat;
			} key;
			const char* text;
		} data;

	public:
		KeyboardEvent(EventType s_type, unsigned int keycode, const char* keyname, bool repeat)
		{
			category = EventCategoryKeyboard;
			type = s_type;
			data.key.keycode = keycode;
			data.key.keyname = keyname;
			data.key.repeat = repeat;
		}
		
		KeyboardEvent(char* text)
		{
			category = EventCategoryKeyboard;
			type = TextInput;
			data.text = text;
		}
	};
}