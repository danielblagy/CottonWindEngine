#pragma once

#include <vector>

#include "layer.h"


namespace cotwin
{
	class LayerStack
	{
	private:
		std::vector<Layer*> layers;
	
	public:
		LayerStack()
		{}

		~LayerStack()
		{
			for (Layer* layer : layers)
				delete layer;
		}

		void push_layer(Layer* layer)
		{
			layers.emplace_back(layer);
		}

		void pop_layer(Layer* layer)
		{
			layers.erase(layers.end());
		}
	};
}