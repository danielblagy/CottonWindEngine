#pragma once

#include "../vendor/entt/entt.hpp"

#include "components.h"

#include "../util/logger.h"

// NOTE : at least for now systems are in scene.update

#include "../input/input.h"
#include "../input/keycodes.h"

#include "../graphics/renderer.h"
#include "../graphics/render_camera_2d.h"

#include <vector>
#include "../physics/2d/collision.h"
#include "../physics/2d/physics.h"

#include "../physics/2d/quadtree.h"


namespace cotwin
{
	class Scene
	{
	public:
		class Entity
		{
		private:
			entt::entity entity_handle = entt::entity(0);
			Scene* scene = NULL;

		public:
			Entity()
			{}

			Entity(entt::entity s_entity_handle, Scene* s_scene)
				: entity_handle(s_entity_handle), scene(s_scene)
			{}

			operator entt::entity() const { return entity_handle; }

			template<typename T, typename... Args>
			T& add_component(Args&&... args)
			{
				if (has_component<T>())
					Logger::Debug("CottonWind \tEntity already has this component!");

				return scene->registry.emplace<T>(entity_handle, std::forward<Args>(args)...);
			}

			template<typename T>
			T& get_component()
			{
				if (!has_component<T>())
					Logger::Debug("CottonWind \tEntity doesn't have this component!");

				return scene->registry.get<T>(entity_handle);
			}

			template<typename T>
			void remove_component()
			{
				if (!has_component<T>())
					Logger::Debug("CottonWind \tEntity doesn't have this component!");

				return scene->registry.remove<T>(entity_handle);
			}

			template<typename T>
			bool has_component()
			{
				return scene->registry.has<T>(entity_handle);
			}
		};

		// A Script Component is a special kind of component
		struct ScriptComponent;

		class ScriptableEntity
		{
		protected:
			Scene::Entity entity;

		public:
			ScriptableEntity()
			{}

			~ScriptableEntity()
			{}

			virtual void on_create() {}
			virtual void on_destroy() {}
			virtual void on_update(float delta) {}

			friend ScriptComponent;
		};

		// TODO : improve the api for adding this component (client has to pass entity as an argument to this component,
		//				even though the component is being added to the entity)
		struct ScriptComponent
		{
			ScriptableEntity* scriptable_entity;

			ScriptComponent(ScriptableEntity* s_scriptable_entity, const Scene::Entity& entity)
			{
				scriptable_entity = s_scriptable_entity;
				scriptable_entity->entity = entity;

				// TODO : call on_create in Scene::create_entity ??
				scriptable_entity->on_create();
			}
		};

		// Collision Resolution Component is a special kind of component
		struct CollisionResolutionComponent
		{
			// TODO : make entity objects be referenced ??
			std::function<void(Entity entity, Entity other)> resolution;

			CollisionResolutionComponent(std::function<void(Entity entity, Entity other)> s_resolution)
				: resolution(s_resolution)
			{}
		};
	
	private:
		entt::registry registry;
		
		// for the collision system
		Quadtree qtree;//(0, glm::vec4{ 0.0f, 0.0f, 1280.0f, 720.0f }, 4);

	public:
		Scene()
		{
			// TODO : have a world info, feed it here as bounds
			qtree = Quadtree(0, glm::vec4(0.0f, 0.0f, 1280.0f, 720.0f), 4);
		}

		~Scene()
		{}

		Entity create_entity(std::string tag)
		{
			Entity entity(registry.create(), this);
			entity.add_component<TagComponent>(tag);
			return entity;
		}

		void destroy_entity(Entity entity)
		{
			if (entity.has_component<ScriptComponent>())
			{
				ScriptComponent& scriptable = entity.get_component<ScriptComponent>();
				scriptable.scriptable_entity->on_destroy();
				delete scriptable.scriptable_entity;
			}

			registry.destroy(entity);
		}
		
		void update(float delta)
		{
			// Script System
			for (auto [entity, script] : registry.view<ScriptComponent>().each())
			{
				script.scriptable_entity->on_update(delta);
			}

			// TODO : move collision system code & physics engine somewhere in here,
			//		  make the physics engine work with constant framerate ??
			//		  and maybe also the collision ??
			
			// Physics system
			// update the physics object struct
			/*for (auto [entity, transform, phys] : registry.view<TransformComponent, PhysicsObjectComponent>().each())
			{
				phys.object.position = transform.center + phys.offset;
				phys.object.min = phys.object.position - phys.size / 2.0f;
				phys.object.max = phys.object.position + phys.size / 2.0f;
				phys.object.velocity = transform.velocity;
			}
			std::vector<std::pair<entt::entity, entt::entity>> processed_pairs;
			// check for collision & resolve if needed
			for (auto [entity, transform, phys] : registry.view<TransformComponent, PhysicsObjectComponent>().each())
			{
				// TODO : "remember" already processed object pairs
				for (auto [other_entity, other_transform, other_phys] : registry.view<TransformComponent, PhysicsObjectComponent>().each())
				{
					if (entity == other_entity)
						continue;

					bool skip = false;
					for (auto [a, b] : processed_pairs)
					{
						if ((entity == a && other_entity == b) || (entity == b && other_entity == a))
							skip = true;
					}

					if (skip)
						continue;

					physics::Manifold manifold = physics::aabb(&phys.object, &other_phys.object);
					// if they collide, resolve the collision
					if (manifold.penetration)
						physics::resolve_impulse(&manifold);

					processed_pairs.push_back(std::make_pair(entity, other_entity));
				}
			}

			// TODO : this is not the best, make a phys component have a pointer to transform velocity
			// update entities' transform velocities if they are physics objects
			for (auto [entity, transform, phys] : registry.view<TransformComponent, PhysicsObjectComponent>().each())
			{
				transform.velocity = phys.object.velocity;
			}*/

			// Transform System
			for (auto [entity, transform] : registry.view<TransformComponent>().each())
			{
				transform.center += transform.velocity;
			}

			// Animation System
			for (auto [entity, sprite, animation] : registry.view<SpriteComponent, AnimationComponent>().each())
			{
				if (sprite.active)
				{
					animation.count += delta;

					if (animation.count >= animation.frequency)
					{
						if (animation.frame >= animation.frames->size())
							animation.frame = 0;

						sprite.texture_rect = animation.frames->at(animation.frame);
						animation.frame++;

						animation.count -= animation.frequency;
					}
				}
			}

			// TODO : maybe provide this class as a convenience script available for use to user, but don't "enforce" it here
			// Camera Controller System
			for (auto [entity, transform, camera] : registry.view<TransformComponent, CameraComponent>().each())
			{
				if (Input::is_key_pressed(CW_KEY_A))
					transform.velocity.x = -120.0f * delta;
				else if (Input::is_key_pressed(CW_KEY_D))
					transform.velocity.x = 120.0f * delta;
				else
					transform.velocity.x = 0.0f;

				if (Input::is_key_pressed(CW_KEY_W))
					transform.velocity.y = -120.0f * delta;
				else if (Input::is_key_pressed(CW_KEY_S))
					transform.velocity.y = 120.0f * delta;
				else
					transform.velocity.y = 0.0f;

				// TODO : make input polling for mouse wheel ??
				// this is just for test
				if (Input::is_key_pressed(CW_KEY_LEFTBRACKET))
				{
					camera.scale += 1.0f * delta;
				}
				else if (Input::is_key_pressed(CW_KEY_RIGHTBRACKET))
				{
					camera.scale -= 1.0f * delta;
				}
			}

			// Audio System
			for (auto [entity, audio_effect] : registry.view<AudioEffectComponent>().each())
			{
				if (audio_effect.play)
				{
					audio_effect.audio.play();
					audio_effect.play = false;
				}
			}
			
			static float count = 0.0f;
			count += delta;

			int sprites_drawn = 0;

			// Sprite Render System
			auto view = registry.view<TransformComponent, SpriteComponent>();
			for (auto [entity, transform, sprite] : view.each()) {
				//Renderer2D::render_texture(sprite.texture, sprite.texture_rect, transform.center, sprite.size);
				
				TransformComponent camera_transform(glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 0.0f});
				CameraComponent camera_info(glm::ivec2{1, 1}, glm::vec2{1.0f, 1.0f});

				// TODO : support multiple cameras in a scene (primary flag), but here determine and use the primary one
				// this loop is expected to iterate only once (until multiple cameras will be supported)
				for (auto [entity, c_transform, c_camera] : registry.view<TransformComponent, CameraComponent>().each())
				{
					camera_transform = c_transform;
					camera_info = c_camera;
				}
				
				if (sprite.active)
				{
					glm::ivec2 camera_half_size = camera_info.bounds / 2.0f;
					RenderCamera render_camera(
						camera_transform.center.x - camera_half_size.x,
						camera_transform.center.y - camera_half_size.y,
						camera_transform.center.x + camera_half_size.x,
						camera_transform.center.y + camera_half_size.y
					);

					// convert rect with left, top, right, bottom
					glm::ivec2 sprite_center = {
						transform.center.x + sprite.center_offset.x,
						transform.center.y + sprite.center_offset.y
					};
					glm::ivec4 sprite_rect = {
						sprite_center.x,
						sprite_center.y,
						sprite_center.x + sprite.size.x,
						sprite_center.y + sprite.size.y
					};

					if (render_camera.captures(sprite_rect))
					{
						// TODO : a position is not scaled properly
						glm::ivec2 sprite_relative_position = {
							camera_info.scale.x * (sprite_rect.x - render_camera.left),
							camera_info.scale.y * (sprite_rect.y - render_camera.top)
						};

						glm::ivec2 sprite_relative_size = {
							sprite.size.x * camera_info.scale.x,
							sprite.size.y * camera_info.scale.y
						};

						Renderer2D::render_texture(sprite.texture, sprite.texture_rect, sprite_relative_position, sprite_relative_size);
						
						sprites_drawn++;
					}
				}
			}

			if (count >= 0.5f)
			{
				count -= 0.5f;
				Logger::Debug("Sprites drawn: %d", sprites_drawn);
			}

			Logger::Debug("Collision checks: %d", physics::collision_checks);
			physics::collision_checks = 0;
			
			// Collision System
			qtree.clear();
			
			// generate a quadtree for this frame
			for (auto [entity, transform, collider]
				: registry.view<TransformComponent, ColliderComponent>().each())
			{
				glm::vec2 collider_origin = transform.center + collider.offset;
				glm::vec4 collider_rect(
					collider_origin.x,
					collider_origin.y,
					collider.size.x,
					collider.size.y
				);

				qtree.insert(Quadtree::Element(entity, collider_rect));
			}

			for (auto [entity, transform, collider, collision_resolution]
				: registry.view<TransformComponent, ColliderComponent, CollisionResolutionComponent>().each())
			{
				glm::vec2 collider_origin = transform.center + collider.offset;
				glm::vec4 collider_rect(
					collider_origin.x,
					collider_origin.y,
					collider.size.x,
					collider.size.y
				);
				
				// TODO : make this a reusable global variable ??
				std::vector<Quadtree::Element> colliding_elements;

				qtree.get_colliding(colliding_elements, Quadtree::Element(entity, collider_rect));

				for (Quadtree::Element& element : colliding_elements)
				{
					collision_resolution.resolution(Entity(entity, this), Entity(element.entity_handle, this));
				}
			}
		}

		void on_window_resize_event(const glm::ivec2& new_size)
		{
			auto view = registry.view<CameraComponent>();
			for (auto [entity, camera] : view.each())
			{
				// update camera scale
				camera.scale.x = (float)new_size.x / (float)camera.bounds.x;
				camera.scale.y = (float)new_size.y / (float)camera.bounds.y;
			}
		}

		friend Entity;
	};
}