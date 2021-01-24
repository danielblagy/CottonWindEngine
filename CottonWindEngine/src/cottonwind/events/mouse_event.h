#pragma once

#include "event.h"
#include "../vendor/glm/glm.hpp"


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
				glm::ivec2 cursor_position;
				glm::ivec2 relative_motion;
			} move;
			// mouse button
			struct
			{
				glm::ivec2 cursor_position;
				unsigned int button_code;
				bool double_clicked;
			} button;
			// mouse wheel
			glm::ivec2 wheel_movement;
		} data;

	public:
		MouseEvent(glm::ivec2 cursor_position, glm::ivec2 relative_motion)
		{
			category = EventCategoryMouse;
			type = MouseMove;
			data.move.cursor_position = cursor_position;
			data.move.relative_motion = relative_motion;
		}

		MouseEvent(EventType s_type, glm::ivec2 cursor_position, unsigned int button_code, bool double_clicked)
		{
			category = EventCategoryMouse;
			type = s_type;
			data.button.cursor_position = cursor_position;
			data.button.button_code = button_code;
			data.button.double_clicked = double_clicked;
		}

		MouseEvent(glm::ivec2 wheel_movement)
		{
			category = EventCategoryMouse;
			type = MouseMove;
			data.wheel_movement = wheel_movement;
		}
	};
}