#pragma once

#include <utility>

#include "../vendor/entt/entt.hpp"

#include "../logger.h"


namespace cotwin
{
	class Entity;
	
	class Scene
	{
	private:
		entt::registry registry;
	
	public:
		Scene()
		{}

		~Scene()
		{}

		// TODO : add base components to entity by default
		Entity create_entity()
		{
			return Entity(registry.create(), this);
		}
		
		void on_update(double delta)
		{}

		friend class Entity;
	};

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
}