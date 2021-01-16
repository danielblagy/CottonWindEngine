#pragma once

#include <SDL.h>


namespace cotwin
{
	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = 1 << 0,
		EventCategoryWindow = 1 << 1,
		EventCategoryKeyboard = 1 << 2,
		EventCategoryMouse = 1 << 3
	};

	enum EventType
	{
		Unknown,
		ApplicationQuit,
		WindowClose, WindowResize, // ...
		KeyPress, KeyRelease, // ...
		MouseMove, MouseButtonPress, MouseButtonRelease, MouseWheel // ...
	};
	
	class Event
	{
	public:
		EventCategory category;
		EventType type;
	};
}