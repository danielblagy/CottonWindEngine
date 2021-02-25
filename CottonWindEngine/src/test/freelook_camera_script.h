#pragma once

#include "../cottonwind/cottonwind.h"


class FreelookCameraScript : public cotwin::Scene::ScriptableEntity
{
public:
	FreelookCameraScript()
		: ScriptableEntity()
	{}

	virtual void on_update(float delta) override
	{
		cotwin::TransformComponent& transform = entity.get_component<cotwin::TransformComponent>();
		cotwin::CameraComponent& camera = entity.get_component<cotwin::CameraComponent>();

		if (camera.primary)
		{
			if (cotwin::Input::is_key_pressed(CW_KEY_A))
				transform.velocity.x = -120.0f * delta;
			else if (cotwin::Input::is_key_pressed(CW_KEY_D))
				transform.velocity.x = 120.0f * delta;
			else
				transform.velocity.x = 0.0f;

			if (cotwin::Input::is_key_pressed(CW_KEY_W))
				transform.velocity.y = -120.0f * delta;
			else if (cotwin::Input::is_key_pressed(CW_KEY_S))
				transform.velocity.y = 120.0f * delta;
			else
				transform.velocity.y = 0.0f;

			// TODO : make input polling for mouse wheel ??
			// this is just for test
			if (cotwin::Input::is_key_pressed(CW_KEY_LEFTBRACKET))
			{
				camera.scale += 1.0f * delta;
				//camera.bounds *= camera.scale;
			}
			else if (cotwin::Input::is_key_pressed(CW_KEY_RIGHTBRACKET))
			{
				camera.scale -= 1.0f * delta;
				//camera.bounds *= camera.scale;
			}
		}
	}
};