#pragma once


namespace cotwin
{
	class State
	{
	protected:
		virtual void on_enter() {}
		virtual void on_update(float delta) {}
	};
}