#pragma once

#include "texture.h"

namespace cotwin
{
	struct Sprite
	{
		// TODO : maybe make this a pointer
		Texture texture;

		glm::ivec4 texture_rect;

		// x, y, width, height;
		glm::ivec4 rect;
	};
}