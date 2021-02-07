#pragma once

#include "../../vendor/glm/glm.hpp"

#include <vector>

#include "../../util/logger.h"


namespace cotwin { namespace physics
{
	struct Object
	{
		glm::vec2 min;	// left, top
		glm::vec2 max;	// right, bottom
		glm::vec2 position;
		glm::vec2 velocity;

		float mass = 0.0f;	// 0 for infinite mass
		float restitution = 0.0f;

		Object()
		{}
		
		Object(const glm::vec2& s_min, const glm::vec2& s_max, float s_mass, float s_restitution)
			: min(s_min), max(s_max), position(min + (max - min) / 2.0f), mass(s_mass), restitution(s_restitution)
		{}
	};

	// data about a collision
	struct Manifold
	{
		Object* a;
		Object* b;
		float penetration;
		glm::vec2 collision_normal;

		Manifold(Object* s_a, Object* s_b)
			: a(s_a), b(s_b), penetration(0.0f)
		{}
	};

	Manifold aabb(Object* a, Object* b)
	{
		Manifold manifold(a, b);

		glm::vec2 vec_ab = b->position - a->position;
		
		// calculate half extents along both axis for each object
		glm::vec2 a_extent = (a->max - a->min) / 2.0f;
		glm::vec2 b_extent = (b->max - b->min) / 2.0f;

		glm::vec2 overlap = a_extent + b_extent - abs(vec_ab);

		// if collide
		if (overlap.x > 0.0f && overlap.y > 0.0f)
		{
			Logger::Debug("aabb manifold generation: collision detected");
			
			// which axis is the axis of least penetration
			if (overlap.x > overlap.y)
			{
				if (vec_ab.x < 0.0f)
					manifold.collision_normal = glm::vec2(-1.0f, 0.0f);
				else
					manifold.collision_normal = glm::vec2(1.0f, 0.0f);
				
				manifold.penetration = overlap.x;
			}
			else
			{
				if (vec_ab.y < 0.0f)
					manifold.collision_normal = glm::vec2(0.0f, -1.0f);
				else
					manifold.collision_normal = glm::vec2(0.0f, 1.0f);

				manifold.penetration = overlap.y;
			}
		}

		return manifold;
	}

	// impulse collision resolution
	void resolve_impulse(Manifold* manifold)
	{
		Logger::Debug("resolve impulse function call");
		
		Object* a = manifold->a;
		Object* b = manifold->b;
		
		// relative velocity
		glm::vec2 vel_ab = b->velocity = a->velocity;

		// relative velocity in terms of the normal direction
		float vel_along_normal = glm::dot(vel_ab, manifold->collision_normal);

		// don't resolve if velocities are separating (intuitive for humans effect)
		if (vel_along_normal > 0.0f)
			return;

		Logger::Debug("resolve impulse function: velocities aren't separating");

		// calculate restitution to use (min one is used)
		float e = glm::min(a->restitution, b->restitution);

		float a_inv_mass = 0.0f;
		if (a->mass != 0.0f)
			a_inv_mass = 1.0f / a->mass;

		float b_inv_mass = 0.0f;
		if (b->mass != 0.0f)
			b_inv_mass = 1.0f / b->mass;
		
		// calculate an impulse scalar
		float j = (-(1 + e) + vel_along_normal) / (a_inv_mass + b_inv_mass);

		// apply the impulse
		glm::vec2 impulse = j * manifold->collision_normal;
		a->velocity -= a_inv_mass * impulse;
		b->velocity -= b_inv_mass * impulse;
	}

	class World
	{
	private:
		std::vector<Object*> objects;

	public:
		void add_object(Object* object)
		{
			objects.push_back(object);
		}

		void remove_object(Object* object)
		{
			objects.erase(std::find(objects.begin(), objects.end(), object));
		}

		void update(float delta)
		{
			for (Object* object : objects)
			{
				// TODO : "remember" already processed object pairs
				for (Object* other : objects)
				{
					if (object == other)
						continue;
					
					Manifold manifold = aabb(object, other);
					// if they collide, resolve the collision
					if (manifold.penetration)
						resolve_impulse(&manifold);
				}
			}
		}
	};
} }