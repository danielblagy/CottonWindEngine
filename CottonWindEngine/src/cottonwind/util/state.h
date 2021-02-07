#pragma once


namespace cotwin
{
	class State
	{
	protected:
		virtual void on_init() {}
		virtual void on_destroy() {}

		// called when the state is being transitioned to
		virtual void on_transition() {}
		virtual void on_update() {}
	};
}