#pragma once

#include "event.h"

#include "../math.h"


namespace cotwin
{
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