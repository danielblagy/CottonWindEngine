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
		std::cout << "test game update" << std::endl;
		// TODO : handle events instead
		if (!running)
			stop();
	}

	void on_destroy() override
	{
		std::cout << "test game destroy" << std::endl;
	}
};

int main(int argc, char* args[])
{
	TestGame game({ "Test Game", 0, 0, 1280, 720, false, true, true, false });
	//TestGame game({ "Test Game", 50, 50, 1280, 720, false, false, false, true });
	//TestGame game({ "Test Game", 0, 0, 1280, 720, true, false, false, false });

	//game.set_target_delta(0.033);
	game.set_target_fps(60);
	
	game.start();
	
	return 0;
}