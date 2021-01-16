#include <iostream>

#include "cottonwind/game.h"


class TestGame : public cotwin::Game
{
public:
	TestGame(cotwin::WindowProperties window_properties)
		: Game(window_properties)
	{
		std::cout << "test game constructor" << std::endl;
	}

	void on_init() override
	{
		std::cout << "test game init" << std::endl;
	}

	void on_update(double delta) override
	{
		cotwin::Event event;
		while (poll_event(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				std::cout << "TestGame: " << SDL_GetKeyName(event.key.keysym.sym) << " was pressed!" << std::endl;
			}
		}
	}

	void on_destroy() override
	{
		std::cout << "test game destroy" << std::endl;
	}
};

int main(int argc, char* args[])
{
	TestGame game(cotwin::WindowProperties("Test Game", 0, 0, 1280, 720, cotwin::Centered | cotwin::Resizable));
	//TestGame game(cotwin::WindowProperties("Test Game", 50, 250, 1280, 720, cotwin::Borderless));
	//TestGame game(cotwin::WindowProperties("Test Game", 0, 0, 1280, 720, cotwin::Fullscreen));

	//game.set_target_delta(0.033);
	game.set_target_fps(60);
	
	game.start();
	
	return 0;
}