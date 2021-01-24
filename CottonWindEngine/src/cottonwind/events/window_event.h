#pragma once

#include "event.h"

#include "../vendor/glm/glm.hpp"


namespace cotwin
{
	class WindowEvent : public Event
	{
	public:
		union
		{
			// window move
			glm::ivec2 new_position;
			// window resize
			glm::ivec2 new_size;
			// just a generic name
			glm::ivec2 data;
		} data;

	public:
		WindowEvent(EventType s_type)
		{
			category = EventCategoryWindow;
			type = s_type;
		}
		
		WindowEvent(EventType s_type, glm::ivec2 s_data)
		{
			category = EventCategoryWindow;
			type = s_type;
			data.data = s_data;
		}
	};
}