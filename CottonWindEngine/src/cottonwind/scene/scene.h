#pragma once

#include "../vendor/flecs/flecs.h"

#include "components.h"
#include "systems.h"

#include "../util/logger.h"


namespace cotwin
{
	class Scene
	{
	private:
		flecs::world world;
		//CameraSystem* camera_system;
		//ColliderSystem* collider_system;

		// used for collision querying from ColliderSystem
		std::vector<std::pair<Entity*, Entity*>> collisions;
	
	public:
		Scene()
		{
			// register the components
			//world.component<TagComponent>();
			//world.component<AudioEffectComponent>();
			//world.component<TransformComponent>();
			//world.component<CameraComponent>();
			//world.component<SpriteComponent>();
			//world.component<AnimationComponent>();
			//world.component<MovementControlComponent>();
			//world.component<ScriptComponent>();
			//world.component<ColliderComponent>();
			
			// TODO : why doesn't system.each compile? (only .iter compiles)
			world.system<ScriptComponent>().iter(ScriptSystem);
			world.system<TransformComponent, MovementControlComponent>().iter(MovementControlSystem);
			world.system<TransformComponent>("TransformSystem").iter(TransformSystem);
			
			world.system<TransformComponent, CameraComponent>("CameraSystem").iter(CameraSystem);
			
			world.system<TransformComponent, CameraComponent>().iter(CameraControllerSystem);
			world.system<SpriteComponent, AnimationComponent>().iter(AnimationSystem);
			world.system<TransformComponent, SpriteComponent>().iter(SpriteRenderSystem);
			world.system<AudioEffectComponent>().iter(AudioSystem);
			
			world.system<TransformComponent, ColliderComponent>().iter(CollisionSystem);
		}

		~Scene()
		{}

		Entity create_entity(std::string tag)
		{
			return world.entity(tag);
		}
		
		void on_update(float delta)
		{
			world.progress(delta);
		}

		void on_window_resize_event(const glm::ivec2& new_size)
		{
			//camera_system->on_window_resize(new_size);
			//world.system("CameraSystem").enable();
		}

		// a conveniance function that returns a sub-vector of collisions of entities with two specified tags
		/*std::vector<std::pair<Entity*, Entity*>>& get_collisions(std::string t1, std::string t2)
		{
			collisions.clear();
			
			for (std::pair<Entity*, Entity*>& collision : collider_system->collisions)
			{
				// TODO : now order of tags matters, is that a right choice ??
				if (collision.first->get<TagComponent>()->tag == t1 && collision.second->get<TagComponent>()->tag == t2)
					collisions.push_back(collision);
			}

			return collisions;
		}*/
	};
}