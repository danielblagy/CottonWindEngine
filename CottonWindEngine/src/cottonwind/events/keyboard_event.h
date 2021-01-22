#pragma once

#include "event.h"


namespace cotwin
{
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