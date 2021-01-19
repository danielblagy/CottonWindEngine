#pragma once

#include <SDL.h>


namespace cotwin
{
	enum EventCategory
	{
		EventCategoryNone = 0,
		EventCategoryWindow = 1 << 0,
		EventCategoryKeyboard = 1 << 1,
		EventCategoryMouse = 1 << 2
	};

	enum EventType
	{
		Unsupported,
		ApplicationQuit,
		WindowClose, WindowResize, WindowFocusGained, WindowFocusLost, WindowMove, WindowMinimize, WindowMaximize,
		KeyPress, KeyRelease, TextInput,
		MouseMove, MouseButtonPress, MouseButtonRelease, MouseWheel
	};
	
	class Event
	{
	public:
		EventCategory category;
		EventType type;
		bool processed = false;

		virtual ~Event() = default;
	};
}