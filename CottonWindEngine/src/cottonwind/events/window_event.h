#pragma once

#include "event.h"

#include "../math.h"


namespace cotwin
{
	class WindowMoveEvent : public Event
	{
	public:
		Vector2i new_position;

	public:
		WindowMoveEvent(Vector2i s_new_position)
			: new_position(s_new_position)
		{
			category = EventCategoryWindow;
			type = WindowMove;
		}
	};

	class WindowResizeEvent : public Event
	{
	public:
		Vector2i new_size;

	public:
		WindowResizeEvent(Vector2i s_new_size)
			: new_size(s_new_size)
		{
			category = EventCategoryWindow;
			type = WindowResize;
		}
	};
}