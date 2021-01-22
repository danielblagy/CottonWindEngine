#pragma once

#include "event.h"
#include "../math.h"


namespace cotwin
{
	// TODO : not one class for any mouse event, but different classes for each event type of Mouse category ??
	
	/*class MouseEvent : public Event
	{
	public:
		Vector2i cursor_position;

	protected:
		MouseEvent(Vector2i s_cursor_position)
			: cursor_position(s_cursor_position)
		{
			category = EventCategoryMouse;
		}
	};

	class MouseMoveEvent : public MouseEvent
	{
	public:
		Vector2i relative_motion;

	public:
		MouseMoveEvent(Vector2i s_cursor_position, Vector2i s_relative_motion)
			: MouseEvent(s_cursor_position), relative_motion(s_relative_motion)
		{
			type = MouseMove;
		}
	};

	class MouseButtonEvent : public MouseEvent
	{
	public:
		unsigned int button_code;
		bool double_clicked;

	public:
		MouseButtonEvent(Vector2i s_cursor_position, unsigned int s_button_code, bool s_double_clicked)
			: MouseEvent(s_cursor_position), button_code(s_button_code), double_clicked(s_double_clicked)
		{}
	};

	class MouseWheelEvent : public Event
	{
	public:
		// x is the amount scrolled horizontally, positive to the right and negative to the left
		// y is the amount scrolled vertically, positive away from the user and negative towards the user
		Vector2i wheel_scroll;

	public:
		MouseWheelEvent(Vector2i s_wheel_scroll)
			: wheel_scroll(s_wheel_scroll)
		{
			category = EventCategoryMouse;
			type = MouseWheel;
		}
	};*/

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