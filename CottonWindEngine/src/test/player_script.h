#pragma once

#include "../cottonwind/cottonwind.h"


// set update logic for the player entity
class PlayerScript : public cotwin::Scene::ScriptableEntity
{
private:
	// scene here is require for scene.get_collisions function, so this will be removed later,
	// as collision resolution will be a part of ScriptableEntity virtual function
	cotwin::Scene& scene;
	std::vector<glm::ivec4> player_idle_frames;
	std::vector<glm::ivec4> player_running_right_frames;
	std::vector<glm::ivec4> player_running_left_frames;
	std::vector<glm::ivec4> player_running_down_frames;
	std::vector<glm::ivec4> player_running_up_frames;
	cotwin::Text collision_detected_message;

public:
	PlayerScript(cotwin::Scene& s_scene)
		: ScriptableEntity(), scene(s_scene)
	{}
	
	virtual void on_create() override
	{
		cotwin::Logger::Info("PlayerScript on create!");
		
		entity.add_component<cotwin::TransformComponent>(glm::vec2{ 700.0f, 500.0f }, glm::vec2{ 0.0f, 0.0f });
		auto& hero_texture = cotwin::ResourceManager::get_texture("src/test/resources/textures/Hero.bmp");
		entity.add_component<cotwin::SpriteComponent>(
			hero_texture, glm::ivec4{ 0, 0, 0, 0 }, glm::ivec2{ 100, 100 }
		);

		entity.add_component<cotwin::AnimationComponent>(0.5f, &player_idle_frames);

		entity.add_component<cotwin::ColliderComponent>(glm::vec2{ 100.0f, 100.0f });

		entity.add_component<cotwin::Scene::CollisionResolutionComponent>(
			[&](cotwin::Scene::Entity entity, cotwin::Scene::Entity other) {
				if (other.get_component<cotwin::TagComponent>().tag == "sensei")
				{
					cotwin::Renderer2D::render_text(collision_detected_message);
					cotwin::Logger::Info("player & sensei collision!");
				}
			}
		);

		// set player animation frames

		player_running_down_frames.reserve(4);
		player_running_up_frames.reserve(4);
		player_running_left_frames.reserve(4);
		player_running_right_frames.reserve(4);
		player_idle_frames.reserve(1);

		for (int i = 0; i < 4; i++)
		{
			player_running_down_frames.push_back(glm::ivec4{ i * 256, 0, 256, 256 });
			player_running_up_frames.push_back(glm::ivec4{ i * 256, 256, 256, 256 });
			player_running_left_frames.push_back(glm::ivec4{ i * 256, 2 * 256, 256, 256 });
			player_running_right_frames.push_back(glm::ivec4{ i * 256, 3 * 256, 256, 256 });
		}

		player_idle_frames.push_back(glm::ivec4{ 2 * 256, 0, 256, 256 });

		// init once, not on each update
		collision_detected_message = cotwin::Text(
			"Player & sensei collision detected!",
			cotwin::ResourceManager::load_font("src/test/resources/fonts/Lato/Lato-Regular.ttf", 28),
			{ 200, 200, 200, 255 },
			{ 300, 300 }
		);
	}

	virtual void on_destroy() override
	{
		cotwin::Logger::Info("PlayerScript on destroy!");
	}

	virtual void on_update(float delta) override
	{
		cotwin::TransformComponent& transform = entity.get_component<cotwin::TransformComponent>();
		cotwin::AnimationComponent& animation = entity.get_component<cotwin::AnimationComponent>();
		//cotwin::Logger::Debug("fr %f  c  %f  fms %d  fm  %d", animation.frequency, animation.count, animation.frames, animation.frame);

		glm::vec2 old_velocity = transform.velocity;

		if (cotwin::Input::is_key_pressed(CW_KEY_LEFT))
			transform.velocity.x = -150.0f * delta;
		else if (cotwin::Input::is_key_pressed(CW_KEY_RIGHT))
			transform.velocity.x = 150.0f * delta;
		else
			transform.velocity.x = 0.0f;

		if (cotwin::Input::is_key_pressed(CW_KEY_UP))
			transform.velocity.y = -150.0f * delta;
		else if (cotwin::Input::is_key_pressed(CW_KEY_DOWN))
			transform.velocity.y = 150.0f * delta;
		else
			transform.velocity.y = 0.0f;

		//cotwin::Logger::Debug("%f", delta);

		if (old_velocity != transform.velocity)
		{
			if (transform.velocity.x > 0.0f)
				animation.set_animation(&player_running_right_frames);
			else if (transform.velocity.x < 0.0f)
				animation.set_animation(&player_running_left_frames);
			else if (transform.velocity.y > 0.0f)
				animation.set_animation(&player_running_down_frames);
			else if (transform.velocity.y < 0.0f)
				animation.set_animation(&player_running_up_frames);
			else
				animation.set_animation(&player_idle_frames);
		}

		// TODO : collision resolution will be done is a special virtual function, which will be a part of the ScriptableEntity class

		// since there is only one player, scene.get_collisions is expected to return just one collision
		// if the player entity & a sensei entity collide, in this case simply display a text on the screen
		/*for (auto& collision : scene.get_collisions("player", "sensei"))
		{
			cotwin::Renderer2D::render_text(collision_detected_message);
		}*/



		// delete the entity with PlayerScript
		if (cotwin::Input::is_key_pressed(CW_KEY_G))
			scene.destroy_entity(entity);
	}
};