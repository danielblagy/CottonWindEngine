#pragma once

#include "../events/window_event.h"
#include "../events/keyboard_event.h"
#include "../events/mouse_event.h"

#include <map>


namespace cotwin { namespace EventDispatcher {
	template<typename L, typename T>
	static void dispatch(L* layer, Event* event, EventType type, void(L::*handler_function)(T*))
	{
		if (event->type == type)
		{
			(layer->*handler_function)(dynamic_cast<T*>(event));
		}
	}
	
	/*template<typename L>
	class EventDispatcher
	{
	private:
		std::map<EventType, void(L::*)(WindowEvent*)> window_callbacks;
		std::map<EventType, void(L::*)(KeyboardEvent*)> keyboard_callbacks;
		std::map<EventType, void(L::*)(MouseEvent*)> mouse_callbacks;

	public:
		void set_callback(EventType type, void(*window_callback)(WindowEvent*))
		{
			window_callbacks[type] = &L::window_callback;
		}

		void set_callback(EventType type, void(L::*keyboard_callback)(KeyboardEvent*))
		{
			keyboard_callbacks[type] = keyboard_callback;
		}

		void set_callback(EventType type, void(L::*mouse_callback)(MouseEvent*))
		{
			mouse_callbacks[type] = mouse_callback;
		}

		void dispatch(Event* event)
		{
			// map.count returns a number of elements with specific key, and since this container doesn't allow
			// duplicates, count will return 0 if there is no element with such key, or 1 if there is
			if (event->category == EventCategoryWindow && window_callbacks.count(event->type))
			{
				void(L::*window_callback)(WindowEvent*) = window_callbacks[event->type];
				//window_callback(static_cast<WindowEvent*>(event));
			}
			else if (event->category == EventCategoryKeyboard && keyboard_callbacks.count(event->type))
			{
				//keyboard_callbacks[event->type](static_cast<KeyboardEvent*>(event));
			}
			else if (event->category == EventCategoryMouse && mouse_callbacks.count(event->type))
			{
				//mouse_callbacks[event->type](static_cast<MouseEvent*>(event));
			}
		}
	};*/
} }