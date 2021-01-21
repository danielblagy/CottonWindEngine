#pragma once

#include <vector>

#include <glad/glad.h>


namespace cotwin
{
	class IndexBuffer
	{
	private:
		unsigned int buffer_id;
		unsigned int count;

	public:
		IndexBuffer(unsigned int* indices, unsigned int count)
		{}

		~IndexBuffer()
		{}

		void bind()
		{}

		void unbind()
		{}

		inline unsigned int getCount() { return count; }
	};
}