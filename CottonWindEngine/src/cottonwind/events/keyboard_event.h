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
	};
}