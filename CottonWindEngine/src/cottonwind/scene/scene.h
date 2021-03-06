#pragma once

#include "../vendor/entt/entt.hpp"

#include "components.h"

#include "../util/logger.h"

#include "../input/input.h"
#include "../input/keycodes.h"

#include "../graphics/renderer.h"
#include "../graphics/render_camera_2d.h"

#include <vector>
#include "../physics/2d/collision.h"
#include "../physics/2d/physics.h"

#include "../physics/2d/quadtree.h"

#include <unordered_map>

// MULTITHREADING
/*
#include <thread>
#include <atomic>
*/


namespace cotwin
{
	// TODO : world bounds field
	class Scene
	{
	public:
		class Entity;

		// NOTE : the components that are defined inside of Scene class are utilizing Scene::Entity,
		//			so they need to be here (since we are header-only)

		// TODO : improve the api for adding this component (client has to pass entity as an argument to this component,
		//				even though the component is being added to the entity)
		class ScriptableEntity;
		struct ScriptComponent;

		// 2D COMPONENT
		// Collision Resolution Component is a special kind of component
		struct CollisionResolutionComponent;

		// 2D COMPONENT
		// attach this to a camera entity and specify the entity for the camera to follow
		// this will modify the transform component of the camera entity each update so as to "follow"
		// the focus entity
		struct CameraFocusComponent;
	
	public:
		glm::vec4 world_rect;
	
	private:
		entt::registry registry;
		
		// for the collision system
		Quadtree qtree;//(0, glm::vec4{ 0.0f, 0.0f, 1280.0f, 720.0f }, 4);
		Quadtree physics_qtree;//(0, glm::vec4{ 0.0f, 0.0f, 1280.0f, 720.0f }, 4);

		std::unordered_map<SpriteComponent::RenderLayer, std::function<bool(Entity entity1, Entity entity2)>> render_sort_functions;

		// MULTITHREADING
		/*
		std::atomic_int running = false;
		std::atomic_int script_system_update = false;
		std::atomic_int physics_system_update = false;
		std::atomic_int collision_system_update = false;
		float temp_delta = 1.0f;

		std::thread script_thread;
		std::thread physics_thread;
		std::thread collision_thread;
		*/

	public:
		// top, left, width, height
		// the world rect is the dimensions of a 2d world
		// the dimensions are used in quadtrees for collision and physics systems
		// if TilemapComponent is used, and its dimensions are bigger than the dimensions provided, world_rect will be stretched automatically
		Scene(const glm::vec4& s_world_rect)
			: world_rect(s_world_rect)
		{
			qtree = Quadtree(0, world_rect, 4);
			physics_qtree = Quadtree(0, world_rect, 4);

			// MULTITHREADING
			/*
			running = true;

			script_thread = std::thread(&Scene::ScriptSystem, this, 1.0f);
			physics_thread = std::thread(&Scene::PhysicsSystem, this);
			collision_thread = std::thread(&Scene::CollisionSystem, this, 1.0f);
			*/
		}

		~Scene()
		{
			// MULTITHREADING
			/*
			running = false;
			script_thread.join();
			physics_thread.join();
			collision_thread.join();
			*/
		}

		friend Entity;

		Entity create_entity(std::string tag);

		void destroy_entity(Entity entity);

		// since we need to create entities to have solid physics bodies (because of how the physics system is designed),
		// the tilemap & collision map are separate, and the collision map is not a component, but a resource
		// (rendering tilemap without using entities, which is a current approach, is much faster)
		// CALL THIS METHOD AFTER CREATING AN ENTITY WITH TILEMAPCOMPONENT
		void generate_collision_map(const CollisionMap& collision_map);
		
		void update(float delta);

		// call this method within a layer on WindowResize event, providing it with the new window size
		// this method will modify the scale of the camera used in the scene
		void on_window_resize_event(const glm::ivec2& new_size);

		// provide a sorting function, which will sort renderable entities (for now it's only the entities with SpriteComponent)
		// withing a layer
		// the comparison function object must return `true` if the first element is _less_ than the second one, `false` otherwise
		void set_render_sort(SpriteComponent::RenderLayer layer, std::function<bool(Entity entity1, Entity entity2)> render_sort_function);

		// have a custom system (must be called in on_update method)
		template <typename... Components, typename... ExcludeComponents>
		void system(std::function<void(Entity entity)> for_each_entity, ExcludeComponents...);

	private:
		void ScriptSystem(float delta);
		void PhysicsSystem();
		void TransformSystem();
		void AnimationSystem(float delta);
		void AudioSystem();
		void CameraFocusSystem();
		void SpriteRenderSystem(float delta);
		void CollisionSystem(float delta);
	};

	class Scene::Entity
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

	class Scene::ScriptableEntity
	{
	protected:
		Entity entity;

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

	struct Scene::ScriptComponent
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

	struct Scene::CollisionResolutionComponent
	{
		// TODO : make entity objects be referenced ??
		std::function<void(Entity entity, Entity other, float delta)> resolution;

		CollisionResolutionComponent(std::function<void(Entity entity, Entity other, float delta)> s_resolution)
			: resolution(s_resolution)
		{}
	};

	struct Scene::CameraFocusComponent
	{
		Entity focus_entity;
	};

	Scene::Entity Scene::create_entity(std::string tag)
	{
		Entity entity(registry.create(), this);
		entity.add_component<TagComponent>(tag);
		return entity;
	}

	void Scene::destroy_entity(Entity entity)
	{
		if (entity.has_component<ScriptComponent>())
		{
			ScriptComponent& scriptable = entity.get_component<ScriptComponent>();
			scriptable.scriptable_entity->on_destroy();
			delete scriptable.scriptable_entity;
		}

		registry.destroy(entity);
	}

	void Scene::generate_collision_map(const CollisionMap& collision_map)
	{
		for (auto [entity, tilemap] : registry.view<TilemapComponent>().each())
		{
			// if the quadtrees' boundaries are too small for the collision map (the map is bigger),
			// than make them bigger by updating the their bounds and recursively deallocating all the children
			// (since they aren't deallocated for each frame for perfomance reasons)
			const glm::vec4& physics_qtree_bounds = physics_qtree.get_bounds();
			const glm::vec4& collisions_qtree_bounds = qtree.get_bounds();

			// get the biggest bounds rect (area)
			glm::vec4 bounds = {
				glm::min(physics_qtree_bounds.x, collisions_qtree_bounds.x),
				glm::min(physics_qtree_bounds.y, collisions_qtree_bounds.y),
				glm::max(physics_qtree_bounds.z, collisions_qtree_bounds.z),
				glm::max(physics_qtree_bounds.w, collisions_qtree_bounds.w)
			};

			bool needs_updating = false;
			
			if (tilemap.origin.x < bounds.x)
			{
				needs_updating = true;
				bounds.x = static_cast<float>(tilemap.origin.x);
			}

			if (tilemap.origin.y < bounds.y)
			{
				needs_updating = true;
				bounds.y = static_cast<float>(tilemap.origin.y);
			}

			float tilemap_right = static_cast<float>(tilemap.origin.x + tilemap.tiles_count.x * tilemap.tile_size);
			if (tilemap_right > bounds.z)
			{
				needs_updating = true;
				bounds.z = tilemap_right;
			}

			float tilemap_bottom = static_cast<float>(tilemap.origin.y + tilemap.tiles_count.y * tilemap.tile_size);
			if (tilemap_bottom > bounds.w)
			{
				needs_updating = true;
				bounds.w = tilemap_bottom;
			}

			if (needs_updating)
			{
				physics_qtree.recursive_dealloc();
				physics_qtree.set_bounds(bounds);
				
				qtree.recursive_dealloc();
				qtree.set_bounds(bounds);

				// update the scene's world_rect property
				world_rect = bounds;
			}
			
			for (int y = 0; y < tilemap.tiles_count.y; y++)
				for (int x = 0; x < tilemap.tiles_count.x; x++)
				{
					// TODO	: along with static solid objects, support generic colliders (tile world file format)
					if (collision_map.map[x + y * tilemap.tiles_count.x] == '1')
					{
						Entity tile = create_entity("solid tile");

						tile.add_component<TransformComponent>(
							glm::vec2(
								static_cast<float>(tilemap.origin.x + x * tilemap.tile_size),
								static_cast<float>(tilemap.origin.y + y * tilemap.tile_size)
							),
							glm::vec2(0.0f, 0.0f)
							);

						tile.add_component<PhysicsObjectComponent>(
							StaticSolidBody,
							glm::vec2(
								static_cast<float>(tilemap.tile_size), static_cast<float>(tilemap.tile_size)
							)
							);
					}
				}
		}
	}

	void Scene::update(float delta)
	{
		ScriptSystem(delta);

		// TODO : move collision system code & physics engine somewhere in here,
		//		  make the physics engine work with constant framerate ??
		//		  and maybe also the collision ??

		PhysicsSystem();

		// MULTITHREADING
		/*
		SpriteRenderSystem(delta);

		// TODO : these can conflict
		temp_delta = delta;
		script_system_update = true;
		physics_system_update = true;
		collision_system_update = true;

		// wait for the threads to finish
		while (script_system_update || physics_system_update || collision_system_update);
		*/

		TransformSystem();

		AnimationSystem(delta);
		
		AudioSystem();

		CameraFocusSystem();

		SpriteRenderSystem(delta);

		CollisionSystem(delta);
	}

	void Scene::on_window_resize_event(const glm::ivec2& new_size)
	{
		auto view = registry.view<CameraComponent>();
		for (auto [entity, camera] : view.each())
		{
			// update camera scale
			camera.scale.x = (float)new_size.x / (float)camera.bounds.x;
			camera.scale.y = (float)new_size.y / (float)camera.bounds.y;
		}
	}

	void Scene::set_render_sort(SpriteComponent::RenderLayer layer, std::function<bool(Entity entity1, Entity entity2)> render_sort_function)
	{
		render_sort_functions[layer] = render_sort_function;
	}

	template <typename... Components, typename... ExcludeComponents>
	void Scene::system(std::function<void(Entity entity)> for_each_entity, ExcludeComponents...)
	{
		auto view = registry.view<Components...>(ExcludeComponents...);
		for (auto entity : view)
		{
			for_each_entity(Entity(entity, this));
		}
	}

	// Systems

	void Scene::ScriptSystem(float delta)
	{
		// Script System
		for (auto [entity, script] : registry.view<ScriptComponent>().each())
		{
			script.scriptable_entity->on_update(delta);
		}
		
		// MULTITHREADING
		/*
		while (running)
		{
			if (!script_system_update) continue;
			script_system_update = false;
			
			// Script System
			for (auto [entity, script] : registry.view<ScriptComponent>().each())
			{
				script.scriptable_entity->on_update(temp_delta);
			}
		}
		*/
	}

	void Scene::PhysicsSystem()
	{
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

		// MULTITHREADING
		/*
		while (running)
		{
			if (!physics_system_update) continue;
			physics_system_update = false;

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
		}
		*/
	}

	void Scene::TransformSystem()
	{
		// Transform System
		for (auto [entity, transform] : registry.view<TransformComponent>().each())
		{
			transform.center += transform.velocity;
		}
	}

	void Scene::AnimationSystem(float delta)
	{
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
	}

	void Scene::AudioSystem()
	{
		// Audio System
		for (auto [entity, audio_effect] : registry.view<AudioEffectComponent>().each())
		{
			if (audio_effect.play)
			{
				audio_effect.audio.play();
				audio_effect.play = false;
			}
		}
	}
	
	void Scene::CameraFocusSystem()
	{
		// Camera Focus System
		for (auto [entity, transform, camera, camera_focus] : registry.view<TransformComponent, CameraComponent, CameraFocusComponent>().each())
		{
			// if the camera is active
			if (camera.primary)
			{
				transform.center = camera_focus.focus_entity.get_component<TransformComponent>().center;
			}
		}
	}

	void Scene::SpriteRenderSystem(float delta)
	{
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

		// render the tilemap
		for (auto [entity, tilemap] : registry.view<TilemapComponent>().each())
		{
			glm::ivec4 tilemap_rect = {
					tilemap.origin.x,
					tilemap.origin.y,
					tilemap.origin.x + tilemap.tiles_count.x * tilemap.tile_size,
					tilemap.origin.y + tilemap.tiles_count.y * tilemap.tile_size
			};

			if (render_camera.captures(tilemap_rect))
			{
				int x = 0, y = 0;
				for (SpriteComponent& tile : tilemap.tiles)
				{
					if (x == tilemap.tiles_count.x)
					{
						x = 0;
						y++;
					}

					glm::ivec2 tile_position = {
						tilemap.origin.x + x * tilemap.tile_size,
						tilemap.origin.y + y * tilemap.tile_size
					};

					glm::ivec4 tile_rect = {
							tile_position.x,
							tile_position.y,
							tile_position.x + tile.size.x,
							tile_position.y + tile.size.y
					};

					if (render_camera.captures(tile_rect))
					{
						glm::ivec2 tile_relative_position = {
							camera_info.scale.x * (tile_rect.x - render_camera.left),
							camera_info.scale.y * (tile_rect.y - render_camera.top)
						};

						glm::ivec2 tile_relative_size = {
							tile.size.x * camera_info.scale.x,
							tile.size.y * camera_info.scale.y
						};

						Renderer2D::render_texture(tile.texture, tile.texture_rect, tile_relative_position, tile_relative_size);

						sprites_drawn++;
					}

					x++;
				}
			}
		}

		// render sprites
		auto render_group = registry.group<TransformComponent, SpriteComponent>();
		// sort by the render layers
		render_group.sort(
			[&](const entt::entity e1, const entt::entity e2)
			{
				Entity entity1(e1, this);
				Entity entity2(e2, this);

				SpriteComponent& sprite1 = entity1.get_component<SpriteComponent>();
				SpriteComponent& sprite2 = entity2.get_component<SpriteComponent>();

				// if both sprites are in the same layer use user-defined sort function to sort the entities within a layer
				if (sprite1.layer == sprite2.layer)
				{
					// if there is no user-defined sort function for this layer
					if (render_sort_functions.count(sprite1.layer) == 0)
						return false;

					return render_sort_functions[sprite1.layer](entity1, entity2);
				}

				return sprite1.layer < sprite2.layer;
			}
		);

		for (auto [entity, transform, sprite] : render_group.each()) {
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
	}

	void Scene::CollisionSystem(float delta)
	{
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
		
		// MULTITHREADING
		/*
		while (running)
		{
			if (!collision_system_update) continue;
			collision_system_update = false;

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
		*/
	}
}