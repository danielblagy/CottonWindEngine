#pragma once

#include "../../vendor/glm/glm.hpp"

#include <vector>


namespace cotwin { namespace physics
{
	struct Object
	{
		glm::vec2 min;	// left, top
		glm::vec2 max;	// right, bottom
		glm::vec2 position;

		// 0 for infinite mass
		float mass;

		Object(const glm::vec2& s_min, const glm::vec2& s_max)
			: min(s_min), max(s_max), position(min + (max - min) / 2.0f)
		{}
	};

	// data about a collision
	struct Manifold
	{
		Object a;
		Object b;
		float penetration;
		glm::vec2 collision_normal;

		Manifold(Object s_a, Object s_b)
			: a(s_a), b(s_b), penetration(0.0f)
		{}
	};
	
	// test for collision of two rects, rect is left, top, width, height
	bool collide_aabb(const glm::vec4& a, const glm::vec4& b)
	{
		float a_right = a.x + a.z;
		float a_bottom = a.y + a.w;
		float b_right = b.x + b.z;
		float b_bottom = b.y + b.w;

		//return a.x <= b_right && a_right >= b.x && a.y <= b_bottom && a_bottom >= b.y;

		// SAT

		// if there's a horizontal gap
		if (a_right < b.x || b_right < a.x)
			return false;

		// if there's a vertical gap
		if (a_bottom < b.y || b_bottom < a.y)
			return false;

		return true;
	}

	// TODO : make them pointers ??
	Manifold aabb(Object a, Object b)
	{
		Manifold manifold(a, b);

		glm::vec2 vec_ab = b.position - a.position;
		
		// calculate half extents along both axis for each object
		glm::vec2 a_extent = (a.max - a.min) / 2.0f;
		glm::vec2 b_extent = (b.max - b.min) / 2.0f;

		glm::vec2 overlap = a_extent + b_extent - abs(vec_ab);

		// if collide
		if (overlap.x > 0.0f && overlap.y > 0.0f)
		{
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
	void resolve_impulse(Object* a, Object* b)
	{
		
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
			
		}
	};
} }