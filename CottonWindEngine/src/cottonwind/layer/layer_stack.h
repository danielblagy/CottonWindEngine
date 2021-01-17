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
			auto it = std::find(layers.begin(), layers.end(), layer);
			layer->on_detach();
			layers.erase(it);
		}

		std::vector<Layer*>::const_iterator begin()
		{
			return layers.begin();
		}

		std::vector<Layer*>::const_iterator end()
		{
			return layers.end();
		}

		std::vector<Layer*>::const_reverse_iterator rbegin()
		{
			return layers.rbegin();
		}

		std::vector<Layer*>::const_reverse_iterator rend()
		{
			return layers.rend();
		}
	};
}