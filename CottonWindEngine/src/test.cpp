#include "cottonwind/game.h"


int main(int argc, char* args[])
{
	cotwin::Game game("Test", 1280, 720, false);
	
	/*
	MainState mainstate();
	*/
	
	while (game.is_running())
	{
		game.update();
	}
	
	return 0;
}