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
	// TODO : world bounds field
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

		// NOTE : the components that are defined inside of Scene class are utilizing Scene::Entity,
		//			so they need to be here (since we are header-only)
		
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

		// 2D COMPONENT
		// Collision Resolution Component is a special kind of component
		struct CollisionResolutionComponent
		{
			// TODO : make entity objects be referenced ??
			std::function<void(Entity entity, Entity other, float delta)> resolution;

			CollisionResolutionComponent(std::function<void(Entity entity, Entity other, float delta)> s_resolution)
				: resolution(s_resolution)
			{}
		};

		// 2D COMPONENT
		// attach this to a camera entity and specify the entity for the camera to follow
		// this will modify the transform component of the camera entity each update so as to "follow"
		// the focus entity
		struct CameraFocusComponent
		{
			Entity focus_entity;
		};
	
	private:
		entt::registry registry;
		
		// for the collision system
		Quadtree qtree;//(0, glm::vec4{ 0.0f, 0.0f, 1280.0f, 720.0f }, 4);
		Quadtree physics_qtree;//(0, glm::vec4{ 0.0f, 0.0f, 1280.0f, 720.0f }, 4);

	public:
		Scene()
		{
			// TODO : have a world info, feed it here as bounds
			qtree = Quadtree(0, glm::vec4(0.0f, 0.0f, 1280.0f, 720.0f), 4);
			physics_qtree = Quadtree(0, glm::vec4(0.0f, 0.0f, 1280.0f, 720.0f), 4);
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
			physics_qtree.clear();

			// generate a quadtree for this frame
			for (auto [entity, transform, physics_object]
				: registry.view<TransformComponent, PhysicsObjectComponent>().each())
			{
				glm::vec2 collider_origin = transform.center + physics_object.offset;
				glm::vec4 collider_rect(
					collider_origin.x,
					collider_origin.y,
					physics_object.size.x,
					physics_object.size.y
				);

				physics_qtree.insert(Quadtree::Element(entity, collider_rect));
			}

			for (auto [entity, transform, physics_object]
				: registry.view<TransformComponent, PhysicsObjectComponent>().each())
			{
				glm::vec2 collider_origin = transform.center + physics_object.offset;
				glm::vec4 collider_rect(
					collider_origin.x,
					collider_origin.y,
					physics_object.size.x,
					physics_object.size.y
				);

				// TODO : make this a reusable global variable ??
				std::vector<Quadtree::Element> colliding_elements;

				physics_qtree.get_colliding(colliding_elements, Quadtree::Element(entity, collider_rect));

				float	left = collider_rect.x, right = collider_rect.x + collider_rect.z,
						top = collider_rect.y, bottom = collider_rect.y + collider_rect.w;
				
				// Resolve collisions for an object of type StaticSolidBody
				if (physics_object.type == StaticSolidBody)
				{
					for (Quadtree::Element& element : colliding_elements)
					{
						// TODO : refactor ??
						Entity element_entity(element.entity_handle, this);
						cotwin::PhysicsObjectComponent& element_object = element_entity.get_component<PhysicsObjectComponent>();
						cotwin::TransformComponent& element_transform = element_entity.get_component<TransformComponent>();
						// A collision resolution for StaticSolidBody-DynamicSolidBody
						if (element_object.type == DynamicSolidBody && collide_aabb(collider_rect, element.rect))
						{
							float new_x, new_y;
							
							bool no_x = false, no_y = false;
							
							if (element.rect.x + element.rect.z >= right)
							{
								new_x = right;
							}
							else if (element.rect.x <= left)
							{
								new_x = left - element.rect.z;
							}
							else
								no_x = true;
							
							if (element.rect.y <= top)
							{
								new_y = top - element.rect.w;
							}
							else if (element.rect.y + element.rect.w >= bottom)
							{
								new_y = bottom;
							}
							else
								no_y = true;

							// find the smallest penetration
							if (no_y || (!no_x && glm::abs(element.rect.x - new_x) < glm::abs(element.rect.y - new_y)))
							{
								element_transform.center.x -= element.rect.x - new_x;
							}
							else
							{
								element_transform.center.y -= element.rect.y - new_y;
							}
						}
					}
				}
			}

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

			// Audio System
			for (auto [entity, audio_effect] : registry.view<AudioEffectComponent>().each())
			{
				if (audio_effect.play)
				{
					audio_effect.audio.play();
					audio_effect.play = false;
				}
			}

			// Camera Focus System
			for (auto [entity, transform, camera, camera_focus] : registry.view<TransformComponent, CameraComponent, CameraFocusComponent>().each())
			{
				// if the camera is active
				if (camera.primary)
				{
					transform.center = camera_focus.focus_entity.get_component<TransformComponent>().center;
				}
			}
			
			static float count = 0.0f;
			count += delta;

			int sprites_drawn = 0;

			// Sprite Render System
			TransformComponent camera_transform;
			CameraComponent camera_info;

			// Get the data about the currently active camera
			for (auto [entity, c_transform, c_camera] : registry.view<TransformComponent, CameraComponent>().each())
			{
				if (c_camera.primary)
				{
					camera_transform = c_transform;
					camera_info = c_camera;
				}
			}

			glm::vec2 camera_half_size = camera_info.bounds / 2.0f;
			RenderCamera render_camera(
				static_cast<int>(camera_transform.center.x - camera_half_size.x),
				static_cast<int>(camera_transform.center.y - camera_half_size.y),
				static_cast<int>(camera_transform.center.x + camera_half_size.x),
				static_cast<int>(camera_transform.center.y + camera_half_size.y)
			);
			
			auto view = registry.view<TransformComponent, SpriteComponent>();
			for (auto [entity, transform, sprite] : view.each()) {
				if (sprite.active)
				{
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

			//Logger::Debug("Collision checks: %d", physics::collision_checks);
			//physics::collision_checks = 0;
			
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
					collision_resolution.resolution(Entity(entity, this), Entity(element.entity_handle, this), delta);
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