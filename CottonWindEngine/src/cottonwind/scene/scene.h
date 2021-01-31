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
		
		// used for collision querying from CollisionSystem
		std::vector<std::pair<Entity, Entity>> collisions;

		// collision system (in here, since system context doesn't work, it.param() gives garbage values)
		void CollisionSystem(flecs::iter& it, TransformComponent* transform, ColliderComponent* collider)
		{
			for (auto i : it)
			{
				glm::vec2 collider_origin = transform[i].center + collider[i].offset;
				glm::vec4 collider_rect(
					collider_origin.x,
					collider_origin.y,
					collider[i].size.x,
					collider[i].size.y
				);

				// TODO : start from i + 1 index (so don't iterate over the same collisions twice)
				size_t end = it.count();
				for (size_t j = i + 1; j < it.count(); j++)
				{
					//if (i == j)
					//	continue;

					glm::vec2 other_collider_origin = transform[j].center + collider[j].offset;
					glm::vec4 other_collider_rect(
						other_collider_origin.x,
						other_collider_origin.y,
						collider[j].size.x,
						collider[j].size.y
					);

					if (physics::collide_aabb(collider_rect, other_collider_rect))
					{
						collisions.push_back(std::make_pair(it.entity(i), it.entity(j)));
					}
				}
			}
		}

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
			
			world.system<TransformComponent>().each(TransformSystem);
			world.system<TransformComponent, MovementControlComponent>().each(MovementControlSystem);
			world.system<SpriteComponent, AnimationComponent>().each(AnimationSystem);
			world.system<ScriptComponent>().each(ScriptSystem);
			world.system<TransformComponent, CameraComponent>().each(CameraControllerSystem);
			world.system<AudioEffectComponent>().each(AudioSystem);
			
			world.system<TransformComponent, SpriteComponent>().iter(SpriteRenderSystem);
			
			world.system<TransformComponent, ColliderComponent>().iter(&Scene::CollisionSystem);
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
		std::vector<std::pair<Entity, Entity>> get_collisions(std::string t1, std::string t2)
		{
			std::vector<std::pair<Entity, Entity>> collisions_of_interest;
			
			for (std::pair<Entity, Entity>& collision : collisions)
			{
				// TODO : order of tags doesn't matter , so check each tag for both t1 & t2
				if (collision.first.get<TagComponent>()->tag == t1 && collision.second.get<TagComponent>()->tag == t2)
					collisions_of_interest.push_back(collision);
			}

			return collisions_of_interest;
		}
	};
}