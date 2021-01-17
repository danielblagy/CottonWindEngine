#pragma once

#include "../events/event.h"


namespace cotwin
{
	class Layer
	{
	protected:
		const char* name;
		bool enabled;
	
	public:
		Layer(const char* s_name)
			: name(s_name), enabled(true)
		{}

		virtual ~Layer() = default;

		virtual void on_attach() {}
		virtual void on_detach() {}
		virtual void on_update(double delta) {}
		virtual void on_event(Event* event) {}

		inline void enable() { enabled = true; }
		inline void disable() { enabled = false; }
	};
}