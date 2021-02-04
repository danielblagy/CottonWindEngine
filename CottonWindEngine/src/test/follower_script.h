#pragma once

#include "../cottonwind/cottonwind.h"


class FollowerScript : public cotwin::Scene::ScriptableEntity
{
private:
	cotwin::Scene::Entity player;

public:
	FollowerScript(cotwin::Scene::Entity s_player)
		: ScriptableEntity(), player(s_player)
	{}
	
	virtual void on_create() override
	{
		cotwin::Logger::Info("Follower entity created!");
	}

	virtual void on_destroy() override
	{
		cotwin::Logger::Info("Follower entity destroyed!");
	}

	float my_length(const glm::vec2& v)
	{
		return sqrt(v.x * v.x + v.y * v.y);
	}
	
	glm::vec2 my_normalize(const glm::vec2& v)
	{
		return v / my_length(v);
	}
	
	virtual void on_update(float delta) override
	{
		cotwin::TransformComponent& follower_transform = entity.get_component<cotwin::TransformComponent>();
		
		glm::vec2 follower_pos = follower_transform.center;
		glm::vec2 player_pos = player.get_component<cotwin::TransformComponent>().center;

		float speed = 100.0f * delta;
		
		glm::vec2 vec = player_pos - follower_pos;
		
		glm::vec2 velocity;
		if (vec.x != 0.0f || vec.y != 0.0f)
		{
			glm::vec2 direction = my_normalize(vec);

			velocity = direction * speed;
		}
		else
			velocity = { 0.0f, 0.0f };

		follower_transform.velocity = velocity;

		//follower_pos += velocity;
	}
};