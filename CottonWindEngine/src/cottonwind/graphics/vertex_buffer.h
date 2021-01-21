#pragma once

#include <vector>

#include <glad/glad.h>

#include "buffer_layout.h"


namespace cotwin
{
	class VertexBuffer
	{
	private:
		unsigned int buffer_id;
		BufferLayout layout;

	public:
		VertexBuffer(float* data, unsigned int size)
		{}

		~VertexBuffer()
		{}

		void setLayout(const BufferLayout& layout)
		{}

		const BufferLayout& getLayout()
		{}

		void bind()
		{}

		void unbind()
		{}
	};
}