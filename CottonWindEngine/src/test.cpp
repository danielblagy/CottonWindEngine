#include <iostream>

#include "cottonwind/game.h"
#include "cottonwind/input/input.h"


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
		
	}

	void on_event(cotwin::Event* event)
	{
		if (event->type == cotwin::ApplicationQuit)
		{
			running = false;
		}
		else if (event->type == cotwin::KeyPress)
		{
			std::cout << "TestGame: " << dynamic_cast<cotwin::KeyboardEvent*>(event)->keyname << " was pressed!" << std::endl;
		}
		else if (event->type == cotwin::MouseButtonPress)
		{
			std::cout << "TestGame: " << dynamic_cast<cotwin::MouseButtonEvent*>(event)->button_code << " mouse button was pressed!" << std::endl;
		}
		else if (event->type == cotwin::WindowMove)
		{
			cotwin::Vector2 new_position = dynamic_cast<cotwin::WindowMoveEvent*>(event)->new_position;
			std::cout << "TestGame: window move (" << new_position.x << ", " << new_position.y << ")" << std::endl;
		}
		else if (event->type == cotwin::WindowResize)
		{
			cotwin::Vector2 new_size = dynamic_cast<cotwin::WindowResizeEvent*>(event)->new_size;
			std::cout << "TestGame: window resize (" << new_size.x << ", " << new_size.y << ")" << std::endl;
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