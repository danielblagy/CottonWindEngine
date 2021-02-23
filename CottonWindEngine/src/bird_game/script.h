#include "../cottonwind/cottonwind.h"


class PlayerScript : public cotwin::Scene::ScriptableEntity
{
private:
	bool menu;
	bool space_was_pressed = false;

	float direction = 1.0f;
	int score = 0;
	
	std::vector<glm::ivec4> right_animation;
	std::vector<glm::ivec4> left_animation;
	
	
	cotwin::Text score_text;
	cotwin::Text menu_text;

public:
	PlayerScript()
		: ScriptableEntity()
	{}

	virtual void on_create() override
	{
		auto& texture = cotwin::ResourceManager::get_texture("src/bird_game/resources/bird.bmp");

		entity.add_component<cotwin::TransformComponent>(glm::vec2{ 310.0f, 410.0f }, glm::vec2{ 0.0f, 0.0f });
		entity.add_component<cotwin::SpriteComponent>(
			texture, glm::ivec4{ 0, 0, 0, 0 }, glm::ivec2{ 80, 80 }
		);

		entity.add_component<cotwin::AnimationComponent>(0.2f, &right_animation);

		entity.add_component<cotwin::ColliderComponent>(glm::vec2{ 80.0f, 80.0f });

		entity.add_component<cotwin::Scene::CollisionResolutionComponent>(
			[&](cotwin::Scene::Entity entity, cotwin::Scene::Entity other, float delta) {
				if (other.get_component<cotwin::TagComponent>().tag == "goal boundary")
				{
					direction *= -1.0f;
					score++;
					score_text.set_text(std::to_string(score));
					entity.get_component<cotwin::AudioEffectComponent>().play = true;
				}
				else if (other.get_component<cotwin::TagComponent>().tag == "death boundary")
				{
					restart();
				}
				else if (other.get_component<cotwin::TagComponent>().tag == "left flame" ||
					other.get_component<cotwin::TagComponent>().tag == "right flame")
				{
					if (other.get_component<cotwin::SpriteComponent>().active)
					{
						restart();
					}
				}
			}
		);

		entity.add_component<cotwin::AudioEffectComponent>(
			cotwin::ResourceManager::get_audio("src/bird_game/resources/point.wav")
		);

		menu = true;
		menu_text = cotwin::Text(
			"Press space to play!",
			cotwin::ResourceManager::load_font("src/test/resources/fonts/Lato/Lato-Regular.ttf", 32),
			{ 0, 0, 0, 255 },
			{ 200, 80 }
		);

		score_text = cotwin::Text(
			"0",
			cotwin::ResourceManager::load_font("src/test/resources/fonts/Lato/Lato-Regular.ttf", 56),
			{ 0, 0, 0, 255 },
			{ 320, 60 }
		);

		right_animation.push_back(glm::ivec4{ 0, 0, 667, 586 });
		right_animation.push_back(glm::ivec4{ 0, 586, 667, 586 });

		left_animation.push_back(glm::ivec4{ 0, 586 * 2, 667, 586 });
		left_animation.push_back(glm::ivec4{ 0, 586 * 3, 667, 586 });
	}

	virtual void on_update(float delta) override
	{
		if (menu)
		{
			cotwin::Renderer2D::render_text(menu_text);
			if (cotwin::Input::is_key_pressed(CW_KEY_SPACE))
				menu = false;
		}
		else
		{
			cotwin::TransformComponent& transform = entity.get_component<cotwin::TransformComponent>();

			cotwin::Renderer2D::render_text(score_text);

			cotwin::AnimationComponent& animation = entity.get_component<cotwin::AnimationComponent>();
			if (direction > 0.0f)
				animation.set_animation(&right_animation);
			else
				animation.set_animation(&left_animation);

			transform.velocity.x = (100.0f + (float)score * 5.0f) * direction * delta;

			float gravity_acceleration = 10.0f * delta;
			transform.velocity.y += gravity_acceleration;

			if (cotwin::Input::is_key_pressed(CW_KEY_SPACE))
			{
				if (!space_was_pressed)
				{
					float jump_force = -350.f * delta;
					transform.velocity.y = jump_force;
				}

				space_was_pressed = true;
			}
			else
				space_was_pressed = false;
		}
	}

private:
	void restart()
	{
		menu = true;
		score = 0;
		score_text.set_text(std::to_string(score));
		
		cotwin::TransformComponent& transform = entity.get_component<cotwin::TransformComponent>();
		transform.center = { 310.0f, 410.0f };
		transform.velocity = { 0.0f, 0.0f };

		direction *= -1.0f;
	}

	friend class FlameScript;
};

class FlameScript : public cotwin::Scene::ScriptableEntity
{
private:
	float* direction;
	float prev_direction = 0.0f;

public:
	FlameScript(cotwin::Scene::Entity player)
		: ScriptableEntity()
	{
		ScriptableEntity* player_state = player.get_component<cotwin::Scene::ScriptComponent>().scriptable_entity;
		direction = &(static_cast<PlayerScript*>(player_state))->direction;
	}

	virtual void on_update(float delta) override
	{
		cotwin::TagComponent& tag = entity.get_component<cotwin::TagComponent>();
		cotwin::TransformComponent& transform = entity.get_component<cotwin::TransformComponent>();
		if (*direction != prev_direction)
		{
			if ((*direction > 0.0f && tag.tag == "left flame") ||
				(*direction < 0.0f && tag.tag == "right flame"))
			{
				entity.get_component<cotwin::SpriteComponent>().active = false;
			}
			else
			{
				entity.get_component<cotwin::SpriteComponent>().active = cotwin::random::b();
			}

			prev_direction = *direction;
		}
	}
};