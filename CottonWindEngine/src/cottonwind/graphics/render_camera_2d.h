#pragma once

#include "../vendor/glm/glm.hpp"


namespace cotwin
{
	// constructed based on CameraComponent
	// used in sprites rendering
	struct RenderCamera
	{
		int left, top, right, bottom;

		RenderCamera(int s_left, int s_top, int s_right, int s_bottom)
			: left(s_left), top(s_top), right(s_right), bottom(s_bottom)
		{}

		// rect is left, top, right, bottom, *NOT* left, top, width, height
		RenderCamera(const glm::vec4& rect)
			: left(rect[0]), top(rect[1]), right(rect[2]), bottom(rect[3])
		{}

		bool captures(int rect_left, int rect_top, int rect_right, int rect_bottom)
		{
			return (rect_right > left || rect_left < right) && (rect_bottom > top || rect_top < bottom);
		}
		
		// rect is left, top, right, bottom, *NOT* left, top, width, height
		bool captures(const glm::ivec4& rect)
		{
			return (rect[2] > left && rect[0] < right) && (rect[3] > top && rect[1] < bottom);
		}
	};
}