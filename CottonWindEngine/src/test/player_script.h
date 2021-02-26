#pragma once

#include "../cottonwind/cottonwind.h"


// set update logic for the player entity
class PlayerScript : public cotwin::Scene::ScriptableEntity
{
private:
	class IdleState : public cotwin::EntityState
	{
	private:
		std::vector<glm::ivec4> player_idle_frames;
	
	public:
		IdleState(const char* id, cotwin::Scene::Entity entity)
			: cotwin::EntityState(id, entity)
		{
			player_idle_frames.push_back(glm::ivec4{ 2 * 256, 0, 256, 256 });
			entity.add_component<cotwin::AnimationComponent>(0.5f, &player_idle_frames);
		}

		virtual void on_enter() override
		{
			cotwin::AnimationComponent& animation = entity.get_component<cotwin::AnimationComponent>();
			animation.set_animation(&player_idle_frames);
		}

		virtual void on_update(float delta) override
		{
			cotwin::TransformComponent& transform = entity.get_component<cotwin::TransformComponent>();
			if (transform.velocity.x != 0.0f || transform.velocity.y != 0.0f)
				state_machine->transition_to_state("running");
		}
	};

	class RunningState : public cotwin::EntityState
	{
	private:
		std::vector<glm::ivec4> player_running_right_frames;
		std::vector<glm::ivec4> player_running_left_frames;
		std::vector<glm::ivec4> player_running_down_frames;
		std::vector<glm::ivec4> player_running_up_frames;

	public:
		RunningState(const char* id, cotwin::Scene::Entity entity)
			: cotwin::EntityState(id, entity)
		{
			player_running_down_frames.reserve(4);
			player_running_up_frames.reserve(4);
			player_running_left_frames.reserve(4);
			player_running_right_frames.reserve(4);

			for (int i = 0; i < 4; i++)
			{
				player_running_down_frames.push_back(glm::ivec4{ i * 256, 0, 256, 256 });
				player_running_up_frames.push_back(glm::ivec4{ i * 256, 256, 256, 256 });
				player_running_left_frames.push_back(glm::ivec4{ i * 256, 2 * 256, 256, 256 });
				player_running_right_frames.push_back(glm::ivec4{ i * 256, 3 * 256, 256, 256 });
			}
		}

		virtual void on_update(float delta) override
		{
			cotwin::TransformComponent& transform = entity.get_component<cotwin::TransformComponent>();
			cotwin::AnimationComponent& animation = entity.get_component<cotwin::AnimationComponent>();
			if (transform.velocity.x > 0.0f)
				animation.set_animation(&player_running_right_frames);
			else if (transform.velocity.x < 0.0f)
				animation.set_animation(&player_running_left_frames);
			else if (transform.velocity.y > 0.0f)
				animation.set_animation(&player_running_down_frames);
			else if (transform.velocity.y < 0.0f)
				animation.set_animation(&player_running_up_frames);
			else
				state_machine->transition_to_state("idle");
		}
	};

private:
	cotwin::Text collision_detected_message;

	cotwin::StateMachine state_machine;

public:
	PlayerScript()
		: ScriptableEntity()
	{}
	
	virtual void on_create() override
	{
		cotwin::Logger::Info("PlayerScript on create!");
		
		entity.add_component<cotwin::TransformComponent>(glm::vec2{ 700.0f, 500.0f }, glm::vec2{ 0.0f, 0.0f });
		auto& hero_texture = cotwin::ResourceManager::get_texture("src/test/resources/textures/Hero.bmp");
		entity.add_component<cotwin::SpriteComponent>(
			hero_texture, glm::ivec4{ 0, 0, 0, 0 }, glm::ivec2{ 100, 100 }
		);

		entity.add_component<cotwin::ColliderComponent>(glm::vec2{ 100.0f, 100.0f });

		entity.add_component<cotwin::Scene::CollisionResolutionComponent>(
			[&](cotwin::Scene::Entity entity, cotwin::Scene::Entity other, float delta) {
				if (other.get_component<cotwin::TagComponent>().tag == "sensei")
				{
					cotwin::Renderer2D::render_text(collision_detected_message);
					cotwin::Logger::Info("player & sensei collision!");
				}
			}
		);

		float collider_height = 100.0f;
		float collider_width = collider_height / 2.0f;
		float collider_offset_x = collider_width / 2.0f;
		float collider_offset_y = 0.0f;
		entity.add_component<cotwin::PhysicsObjectComponent>(
			cotwin::DynamicSolidBody,
			glm::vec2{ collider_width / 2.0f, collider_height },
			glm::vec2{ collider_offset_x, collider_offset_y }
		);

		// init once, not on each update
		collision_detected_message = cotwin::Text(
			"Player & sensei collision detected!",
			cotwin::ResourceManager::load_font("src/test/resources/fonts/Lato/Lato-Regular.ttf", 28),
			{ 200, 200, 200, 255 },
			{ 300, 300 }
		);

		// init register states in state_machine
		state_machine.register_state(new IdleState("idle", entity));
		state_machine.register_state(new RunningState("running", entity));

		// init current state
		state_machine.set_init_state("idle");
	}

	virtual void on_destroy() override
	{
		cotwin::Logger::Info("PlayerScript on destroy!");
	}

	virtual void on_update(float delta) override
	{
		cotwin::TransformComponent& transform = entity.get_component<cotwin::TransformComponent>();

		float speed = 150.0f * delta;
		glm::vec2 vec;
		
		if (cotwin::Input::is_key_pressed(CW_KEY_LEFT))
			vec.x = -1.0f;
		else if (cotwin::Input::is_key_pressed(CW_KEY_RIGHT))
			vec.x = 1.0f;
		else
			vec.x = 0.0f;

		if (cotwin::Input::is_key_pressed(CW_KEY_UP))
			vec.y = -1.0f;
		else if (cotwin::Input::is_key_pressed(CW_KEY_DOWN))
			vec.y = 1.0f;
		else
			vec.y = 0.0f;

		if (vec.x != 0.0f || vec.y != 0.0f)
		{
			glm::vec2 direction = normalize(vec);

			transform.velocity = direction * speed;
		}
		else
		{
			transform.velocity = { 0.0f, 0.0f };
		}

		state_machine.update(delta);

		// TODO : collision resolution will be done is a special virtual function, which will be a part of the ScriptableEntity class
	}
};