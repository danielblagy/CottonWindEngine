#pragma once

#include "event.h"
#include "../math.h"


namespace cotwin
{
	class MouseEvent : public Event
	{
	public:
		union
		{
			// mouse move
			struct
			{
				Vector2i cursor_position;
				Vector2i relative_motion;
			} move;
			// mouse button
			struct
			{
				Vector2i cursor_position;
				unsigned int button_code;
				bool double_clicked;
			} button;
			// mouse wheel
			Vector2i wheel_movement;
		} data;

	public:
		MouseEvent(Vector2i cursor_position, Vector2i relative_motion)
		{
			category = EventCategoryMouse;
			type = MouseMove;
			data.move.cursor_position = cursor_position;
			data.move.relative_motion = relative_motion;
		}

		MouseEvent(EventType s_type, Vector2i cursor_position, unsigned int button_code, bool double_clicked)
		{
			category = EventCategoryMouse;
			type = s_type;
			data.button.cursor_position = cursor_position;
			data.button.button_code = button_code;
			data.button.double_clicked = double_clicked;
		}

		MouseEvent(Vector2i wheel_movement)
		{
			category = EventCategoryMouse;
			type = MouseMove;
			data.wheel_movement = wheel_movement;
		}
	};
}