#pragma once

#include <vector>
#include <cstring>

#include "logger.h"
#include "../scene/scene.h"


namespace cotwin
{
	class StateMachine;
	
	class State
	{
	protected:
		const char* id = 0;
		StateMachine* state_machine = 0;
	
	public:
		State(const char* s_id)
			: id(s_id)
		{}
		
		virtual void on_enter() {}
		virtual void on_leave() {}
		virtual void on_update(float delta) {}

		inline const char* get_id() { return id; }

		friend StateMachine;
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

		~StateMachine()
		{
			for (State* state : states)
				delete state;
		}

		void register_state(State* state)
		{
			if (state)
			{
				states.push_back(state);
				state->state_machine = this;
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
				{
					change_current_state(state);
					break;
				}
		}

		void update(float delta)
		{
			current_state->on_update(delta);
		}

		// called once on init (after registering at least one state)
		void set_init_state(const char* state_id)
		{
			if (current_state)
			{
				Logger::Debug("An initial state has already been set");
			}
			else
			{
				for (State* state : states)
					if (std::strcmp(state->get_id(), state_id) == 0)
					{
						current_state = state;
						current_state->on_enter();
					}
			}
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

	class EntityState : public State
	{
	protected:
		Scene::Entity entity;

	public:
		EntityState(const char* s_id, Scene::Entity s_entity)
			: State(s_id), entity(s_entity)
		{}

		virtual void on_enter() {}
		virtual void on_leave() {}
		virtual void on_update(float delta) {}

		inline const char* get_id() { return id; }
	};
}