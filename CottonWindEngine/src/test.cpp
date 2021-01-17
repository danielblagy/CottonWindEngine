#include <iostream>

#include "cottonwind/game.h"
#include "cottonwind/input/input.h"
#include "cottonwind/input/keycodes.h"
#include "cottonwind/input/mouse_buttons.h"


// layer where the main game logic and render is
class TestMainLayer : public cotwin::Layer
{
private:
	cotwin::Renderer renderer;
	cotwin::Vector4ui8 orange_color = { 255, 165, 0 };
	cotwin::Vector4ui8 yellow_color = { 255, 255, 0 };

public:
	TestMainLayer(cotwin::Renderer s_renderer)
		: cotwin::Layer("main"), renderer(s_renderer)
	{}

	void on_attach() override
	{
		std::cout << "test main layer on attach" << std::endl;
	}

	void on_detach() override
	{
		std::cout << "test main layer on detach" << std::endl;
	}

	void on_update(double delta) override
	{
		renderer.draw_rect({ 50, 50, 50, 50 }, orange_color);
		renderer.fill_rect({ 500, 450, 120, 60 }, yellow_color);
		
		std::cout << "test main layer on update: delta " << delta << " sec" << "  FPS: " << 1.0 / delta << std::endl;
		if (cotwin::Input::is_key_pressed(CW_KEY_F))
			std::cout << "TestGame: JUMP is pressed!" << std::endl;

		if (cotwin::Input::is_mouse_button_pressed(CW_MOUSEBUTTON_LEFT))
			std::cout << "TestGame: SHOOT is pressed!" << std::endl;

		cotwin::Vector2 mouse_position = cotwin::Input::get_mouse_position();
		std::cout << "TestGame: Mouse Position (" << mouse_position.x << ", " << mouse_position.y << ")" << std::endl;
	}

	void on_event(cotwin::Event* event) override
	{
		event->processed = true;

		std::cout << "test main layer on event" << std::endl;
		if (event->type == cotwin::ApplicationQuit)
		{
			// for example, save game state to file
			std::cout << "TestGame: Game is stopping!" << std::endl;
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
		else
			event->processed = false;
	}
};

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
		attach_layer(new TestMainLayer(get_renderer()));
		set_fps_cap(60);
		set_render_clear_color({ 70, 100, 200, 255 });
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
	
	game.start();
	
	return 0;
}