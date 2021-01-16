#pragma once

#include "event.h"
#include "../math.h"


namespace cotwin
{
	// TODO : not one class for any mouse event, but different classes for each event type of Mouse category ??
	
	class MouseEvent : public Event
	{
	public:
		Vector2 cursor_position;
		unsigned int button_code;
		const char* button_name;
		// x is the amount scrolled horizontally, positive to the right and negative to the left
		// y is the amount scrolled vertically, positive away from the user and negative towards the user
		Vector2 wheel_scroll;
	};
}