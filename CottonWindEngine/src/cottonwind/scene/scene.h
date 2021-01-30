#pragma once

#include "../vendor/ecs/ECS.h"

#include "components.h"
#include "systems.h"

#include "../util/logger.h"


namespace cotwin
{
	class Scene
	{
	private:
		ECS::World* world;
		CameraSystem* camera_system;
		ColliderSystem* collider_system;

		// used for collision querying from ColliderSystem
		std::vector<std::pair<Entity*, Entity*>> collisions;
	
	public:
		Scene()
		{
			world = ECS::World::createWorld();
			world->registerSystem(new ScriptSystem());
			world->registerSystem(new MovementControlSystem());
			world->registerSystem(new TransformSystem());
			camera_system = new CameraSystem();
			world->registerSystem(camera_system);
			world->registerSystem(new CameraControllerSystem());
			world->registerSystem(new AnimationSystem());
			world->registerSystem(new SpriteRenderSystem());
			world->registerSystem(new AudioSystem());
			collider_system = new ColliderSystem();
			world->registerSystem(collider_system);
		}

		~Scene()
		{
			world->destroyWorld();
		}

		Entity* create_entity(std::string tag)
		{
			ECS::Entity* entity = world->create();

			entity->assign<TagComponent>(tag);
			
			return entity;
		}
		
		void on_update(float delta)
		{
			world->tick(delta);
		}

		void on_window_resize_event(const glm::ivec2& new_size)
		{
			camera_system->on_window_resize(new_size);
		}

		// a conveniance function that returns a sub-vector of collisions of entities with two specified tags
		std::vector<std::pair<Entity*, Entity*>>& get_collisions(std::string t1, std::string t2)
		{
			collisions.clear();
			
			for (std::pair<Entity*, Entity*>& collision : collider_system->collisions)
			{
				// TODO : now order of tags matters, is that a right choice ??
				if (collision.first->get<TagComponent>()->tag == t1 && collision.second->get<TagComponent>()->tag == t2)
					collisions.push_back(collision);
			}

			return collisions;
		}
	};
}