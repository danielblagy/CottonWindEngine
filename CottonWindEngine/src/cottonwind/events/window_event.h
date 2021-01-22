#pragma once

#include "event.h"

#include "../math.h"


namespace cotwin
{
	/*class WindowMoveEvent : public Event
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
	};*/

	class WindowEvent : public Event
	{
	public:
		union
		{
			// window move
			Vector2i new_position;
			// window resize
			Vector2i new_size;
			// just a generic name
			Vector2i data;
		} data;

	public:
		WindowEvent(EventType s_type, Vector2i s_data)
		{
			category = EventCategoryWindow;
			type = s_type;
			data.data = s_data;
		}
	};
}