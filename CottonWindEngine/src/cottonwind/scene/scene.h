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
		
		CameraSystemContext camera_system_context;
		int test_context = 57;
		
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
			
			//auto camera_system = world.system<TransformComponent, CameraComponent>().iter(CameraSystem);
			//camera_system.set<flecs::Context>({ &test_context });
			
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
			Entity entity = world.entity().set<TagComponent>({ tag });

			return entity;
		}
		
		void on_update(float delta)
		{
			// TODO : make a special method for initializing contexts for systems
			//			and require a client to call it once the camera entity was set
			//		or create a camera entity by default ?????????????????????????????????
			//world.query<TransformComponent, CameraComponent>().iter(
			//	[&](flecs::iter& it, TransformComponent* transform, CameraComponent* camera) {
			//		// this loop is expected to iterate only once
			//		for (auto i : it)
			//		{
			//			sprite_render_system_context.camera_transform = transform[i];
			//			sprite_render_system_context.camera = camera[i];
			//			sprite_render_system_context.a = 2807;
			//		}
			//	}
			//);
			test_context = 112;

			world.progress();
		}

		void on_window_resize_event(const glm::ivec2& new_size)
		{
			world.query<CameraComponent>().iter(
				[&](flecs::iter& it, CameraComponent* camera) {
					for (auto i : it)
					{
						// update camera scale
						camera[i].scale.x = (float)new_size.x / (float)camera[i].bounds.x;
						camera[i].scale.y = (float)new_size.y / (float)camera[i].bounds.y;
					}
				}
			);
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