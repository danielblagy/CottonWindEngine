#pragma once

#include "window_event.h"
#include "keyboard_event.h"
#include "mouse_event.h"

#include <map>


namespace cotwin
{
	class EventDispatcher
	{
	private:
		std::map<EventType, void(*)(WindowEvent*)> window_callbacks;
		std::map<EventType, void(*)(KeyboardEvent*)> keyboard_callbacks;
		std::map<EventType, void(*)(MouseEvent*)> mouse_callbacks;

	public:
		void set_callback(EventType type, void(*window_callback)(WindowEvent*))
		{
			window_callbacks[type] = window_callback;
		}

		void set_callback(EventType type, void(*keyboard_callback)(KeyboardEvent*))
		{
			keyboard_callbacks[type] = keyboard_callback;
		}

		void set_callback(EventType type, void(*mouse_callback)(MouseEvent*))
		{
			mouse_callbacks[type] = mouse_callback;
		}

		void dispatch(Event* event)
		{
			// map.count returns a number of elements with specific key, and since this container doesn't allow
			// duplicates, count will return 0 if there is no element with such key, or 1 if there is
			if (event->category == EventCategoryWindow && window_callbacks.count(event->type))
			{
				window_callbacks[event->type](static_cast<WindowEvent*>(event));
			}
			else if (event->category == EventCategoryKeyboard && keyboard_callbacks.count(event->type))
			{
				keyboard_callbacks[event->type](static_cast<KeyboardEvent*>(event));
			}
			else if (event->category == EventCategoryMouse && mouse_callbacks.count(event->type))
			{
				mouse_callbacks[event->type](static_cast<MouseEvent*>(event));
			}
		}
	};
}