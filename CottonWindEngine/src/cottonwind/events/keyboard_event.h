#pragma once

#include "event.h"


namespace cotwin
{
	// TODO : key pressed repeat event
	
	class KeyboardEvent : public Event
	{
	public:
		unsigned int keycode;
		const char* keyname;
		bool repeat;

	public:
		KeyboardEvent(unsigned int s_keycode, const char* s_keyname, bool s_repeat)
			: keycode(s_keycode), keyname(s_keyname), repeat(s_repeat)
		{
			category = EventCategoryKeyboard;
		}
	};
}