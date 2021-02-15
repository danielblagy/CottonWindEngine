#pragma once

#include "../../vendor/glm/glm.hpp"
#include "../../vendor/entt/entt.hpp"

#include <vector>

#include "collision.h"

#include "../../util/logger.h"


namespace cotwin
{
	class Quadtree
	{
	public:
		struct Element
		{
			entt::entity entity_handle;
			// top, left, width, height
			glm::vec4 rect;

			Element(entt::entity s_entity_handle, const glm::vec4& s_rect)
				: entity_handle(s_entity_handle), rect(s_rect)
			{}
		};
	
	private:
		// will be left as zero if it's the root
		Quadtree* parent = 0;

		// children quads
		Quadtree* child_nw = 0;	// north-west
		Quadtree* child_ne = 0;	// north-east
		Quadtree* child_se = 0;	// south-east
		Quadtree* child_sw = 0;	// south-west

		// top, left, width, height
		glm::vec4 bounds;
		int capacity;	// if capacity reached, subdivide the quad
						// (if an element is an edge case, the element will be inserted in this node's elements array, event if capacity is reached)
		std::vector<Element> elements;

		bool divided = false;

	public:
		Quadtree()
			: parent(0), bounds(glm::vec4(0.0f, 0.0f, 100.0f, 100.0f)), capacity(4)
		{}
		
		Quadtree(Quadtree* s_parent, const glm::vec4& s_bounds, int s_capacity)
			: parent(s_parent), bounds(s_bounds), capacity(s_capacity)
		{}

		~Quadtree()
		{
			delete child_nw;
			delete child_ne;
			delete child_se;
			delete child_sw;
		}

		void insert(const Element& element)
		{
			if (elements.size() < capacity)
			{
				elements.push_back(element);
				return;
			}
			
			if (!divided)
			{
				// subdivide the quad into 4 new smaller quads
				subdivide();
			}

			float vertical_line = bounds.x + bounds.z / 2.0f;
			float horizontal_line = bounds.y + bounds.w / 2.0f;
			
			// check if an element is totally contained inside w, e, n, s
			bool west = element.rect.x + element.rect.z < vertical_line;
			bool east = element.rect.x > vertical_line;
			bool north = element.rect.y + element.rect.w < horizontal_line;
			bool south = element.rect.y > horizontal_line;
			
			// if an element is on the edge, add it to the current node (to both not duplicate and have proper collision detection)
			if ((!west && !east) || (!north && !south))
			{
				elements.push_back(element);
			}
			else
			{
				if (north)
				{
					if (west)
						child_nw->insert(element);
					else if (east)
						child_ne->insert(element);
					else
						// NOTE : this should never happen (how could it ??)
						Logger::Debug("Quadtree: isertion failed, an element is neither on the edges nor in quads");
				}
				else if (south)
				{
					if (east)
						child_se->insert(element);
					else if (west)
						child_sw->insert(element);
					else
						// NOTE : this should never happen (how could it ??)
						Logger::Debug("Quadtree: isertion failed, an element is neither on the edges nor in quads");
				}
			}
		}

		// clears elements array
		void clear()
		{
			elements.clear();

			if (divided)
			{
				child_nw->clear();
				child_ne->clear();
				child_se->clear();
				child_sw->clear();
			}

			// NOTE : the children are not deallocated to save time (next frame allocs & bounds calculation)
		}

		// get the elements that may be colliding with an element
		void get_potentially_colliding(std::vector<Element>& elements_array, const Element& element)
		{
			for (Element& e : elements)
				if (e.entity_handle != element.entity_handle && physics::collide_aabb(e.rect, element.rect))
					elements_array.push_back(e);

			// if children are allocated (exist)
			if (divided)
			{
				if (physics::collide_aabb(child_nw->bounds, element.rect))
					child_nw->get_potentially_colliding(elements_array, element);

				if (physics::collide_aabb(child_ne->bounds, element.rect))
					child_ne->get_potentially_colliding(elements_array, element);

				if (physics::collide_aabb(child_se->bounds, element.rect))
					child_se->get_potentially_colliding(elements_array, element);

				if (physics::collide_aabb(child_sw->bounds, element.rect))
					child_sw->get_potentially_colliding(elements_array, element);
			}
		}

	private:
		void subdivide()
		{
			glm::vec2 size = { bounds.z / 2.0f, bounds.w / 2.0f };

			glm::vec4 nw_rect = { bounds.x, bounds.y, size.x, size.y };
			glm::vec4 ne_rect = { bounds.x + size.x, bounds.y, size.x, size.y };

			glm::vec4 se_rect = { bounds.x + size.x, bounds.y + size.y, size.x, size.y };
			glm::vec4 sw_rect = { bounds.x, bounds.y + size.y, size.x, size.y };

			child_nw = new Quadtree(this, nw_rect, capacity);
			child_ne = new Quadtree(this, ne_rect, capacity);
			child_se = new Quadtree(this, se_rect, capacity);
			child_sw = new Quadtree(this, sw_rect, capacity);

			divided = true;
		}
	};
}