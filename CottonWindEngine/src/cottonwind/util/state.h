#pragma once

#include <vector>
#include <cstring>

#include "logger.h"


namespace cotwin
{
	class State
	{
	protected:
		const char* id = 0;
	
	public:
		State(const char* s_id)
			: id(s_id)
		{}
		
		virtual void on_enter() {}
		virtual void on_leave() {}
		virtual void on_update(float delta) {}

		inline const char* get_id() { return id; }
	};

	// TODO : check if current_state is null, or simply log if it is
	class StateMachine
	{
	private:
		std::vector<State*> states;
		State* current_state = 0;
	
	public:
		StateMachine()
		{}

		void register_state(State* state)
		{
			if (state)
			{
				states.push_back(state);
			}
			else
			{
				Logger::Debug("Passing null to StateMachine::register_state");
			}
		}

		void transition_to_state(const char* state_id)
		{
			for (State* state : states)
				if (std::strcmp(state->get_id(), state_id) == 0)
					change_current_state(state);
		}

		void update(float delta)
		{
			current_state->on_update(delta);
		}

	private:
		void change_current_state(State* state)
		{
			if (state)
			{
				current_state->on_leave();
				current_state = state;
				current_state->on_enter();
			}
			else
			{
				Logger::Debug("A state in StateMachine::states was deallocated");
			}
		}
	};
}