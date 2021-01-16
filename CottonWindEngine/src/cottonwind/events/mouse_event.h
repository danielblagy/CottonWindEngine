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

	protected:
		MouseEvent(Vector2 s_cursor_position)
			: cursor_position(s_cursor_position)
		{}
	};

	class MouseMovedEvent : public MouseEvent
	{
	public:
		Vector2 relative_motion;

	public:
		MouseMovedEvent(Vector2 s_cursor_position, Vector2 s_relative_motion)
			: MouseEvent(s_cursor_position), relative_motion(s_relative_motion)
		{}
	};

	class MouseButtonEvent : public MouseEvent
	{
	public:
		unsigned int button_code;
		bool double_clicked;

	public:
		MouseButtonEvent(Vector2 s_cursor_position, unsigned int s_button_code, bool s_double_clicked)
			: MouseEvent(s_cursor_position), button_code(s_button_code), double_clicked(s_double_clicked)
		{}
	};

	class MouseWheelEvent : public MouseEvent
	{
	public:
		// x is the amount scrolled horizontally, positive to the right and negative to the left
		// y is the amount scrolled vertically, positive away from the user and negative towards the user
		Vector2 wheel_scroll;

	public:
		MouseWheelEvent(Vector2 s_cursor_position, Vector2 s_wheel_scroll)
			: MouseEvent(s_cursor_position), wheel_scroll(s_wheel_scroll)
		{}
	};
}