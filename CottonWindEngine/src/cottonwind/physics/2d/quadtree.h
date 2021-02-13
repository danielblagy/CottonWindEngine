#pragma once

#include "../../vendor/glm/glm.hpp"

#include <vector>

#include "collision.h"


namespace cotwin
{
	// a component for the scene
	struct QTElementComponent
	{
		// to quickly tell if two objects are different, unique for each object
		unsigned int id;
		glm::vec4 rect;
	};
	
	class Quadtree
	{
	private:
		// will be left as zero if it's the root
		Quadtree* parent = 0;

		// children quads
		Quadtree* child_nw = 0;	// north-west
		Quadtree* child_ne = 0;	// north-east
		Quadtree* child_se = 0;	// south-east
		Quadtree* child_sw = 0;	// south-west

		// x,y - center, half_width, half_height
		glm::vec4 bounds;
		int capacity;	// if capacity reached, subdivide the quad
		// TODO : store entities instead ??
		std::vector<QTElementComponent> elements;

		bool divided = false;

	public:
		Quadtree(Quadtree* s_parent, const glm::vec4& s_bounds, int s_capacity)
			: parent(s_parent), bounds(s_bounds), capacity(s_capacity)
		{}

		bool insert(const QTElementComponent& element)
		{
			// TODO : this conversion is terrible, find another way
			glm::vec4 bounds_converted = {
				bounds.x - bounds.z,
				bounds.y - bounds.w,
				bounds.z * 2.0f,
				bounds.w * 2.0f
			};
			glm::vec4 element_rect_converted = {
				element.rect.x - element.rect.z,
				element.rect.y - element.rect.w,
				element.rect.z * 2.0f,
				element.rect.w * 2.0f
			};
			
			// TODO : edge cases ??
			if (!physics::collide_aabb(bounds_converted, element_rect_converted))
			{
				return false;
			}
			
			if (elements.size() < capacity)
			{
				elements.push_back(element);
				return true;
			}
			
			if (!divided)
			{
				// subdivide the quad into 4 new smaller quads
				subdivide();
			}

			// so that an element won't be addded twice if it's on the edge
			if (child_nw->insert(element)) return true;
			if (child_ne->insert(element)) return true;
			if (child_se->insert(element)) return true;
			if (child_sw->insert(element)) return true;
		}

	private:
		void subdivide()
		{
			glm::vec2 size = { bounds.z / 2.0f, bounds.w / 2.0f };

			glm::vec4 nw_rect = { bounds.x - size.x, bounds.y - size.y, size.x, size.y };
			glm::vec4 ne_rect = { bounds.x + size.x, bounds.y - size.y, size.x, size.y };

			glm::vec4 sw_rect = { bounds.x - size.x, bounds.y + size.y, size.x, size.y };
			glm::vec4 se_rect = { bounds.x + size.x, bounds.y + size.y, size.x, size.y };

			child_nw = new Quadtree(this, nw_rect, capacity);
			child_ne = new Quadtree(this, ne_rect, capacity);
			child_se = new Quadtree(this, se_rect, capacity);
			child_sw = new Quadtree(this, sw_rect, capacity);

			divided = true;
		}
	};
}