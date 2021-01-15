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

	void on_init()
	{
		std::cout << "test game init" << std::endl;
	}

	void on_update()
	{
		std::cout << "test game update" << std::endl;
		// TODO : handle events instead
		if (!running)
			stop();
	}

	void on_destroy()
	{
		std::cout << "test game destroy" << std::endl;
	}
};

int main(int argc, char* args[])
{
	// TODO : test other window properties
	TestGame game({ "Test Game", 0, 0, 1280, 720, false, true, true, false });
	
	game.start();
	
	return 0;
}