#pragma once

#include "sprite.h"


namespace cotwin
{
	class Animation
	{
	private:
		Sprite& sprite;
		// in seconds
		float frequency;
		float count;
		std::vector<glm::ivec4> frames;
		std::size_t i;

	public:
		Animation(Sprite& s_sprite, float s_frequency)
			: sprite(s_sprite), frequency(s_frequency), count(0.0f)
		{}

		Animation(Sprite& s_sprite, float s_frequency, std::vector<glm::ivec4> s_frames)
			: sprite(s_sprite), frequency(s_frequency), count(0.0f), frames(s_frames)
		{
			refresh();
		}

		~Animation()
		{}

		void add_frame(glm::ivec4& frame)
		{
			frames.push_back(frame);
		}

		void refresh()
		{
			i = 0;
			sprite.texture_rect = frames.at(i);
			i++;
		}

		void update(float delta)
		{
			count += delta;

			if (count >= frequency)
			{
				sprite.texture_rect = frames.at(i);
				i++;
				if (i >= frames.size())
					refresh();
				count -= frequency;
			}
		}
	};
}