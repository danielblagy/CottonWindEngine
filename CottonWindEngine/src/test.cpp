#include <iostream>

#define CW_DEBUG_MODE_ENABLED 1

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

	// gui state
	ImVec4 color;
	bool show_demo_window;
	bool show_another_window;

public:
	TestMainLayer(cotwin::Renderer s_renderer)
		: cotwin::Layer("main"), renderer(s_renderer)
	{}

	void on_attach() override
	{
		std::cout << "test main layer on attach" << std::endl;

		color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		show_demo_window = true;
		show_another_window = false;
	}

	void on_detach() override
	{
		std::cout << "test main layer on detach" << std::endl;
	}

	void on_update(double delta) override
	{
		renderer.draw_rect({ 50, 50, 50, 50 }, orange_color);
		renderer.fill_rect({ 500, 450, 120, 60 }, yellow_color);
		
		if (cotwin::Input::is_key_pressed(CW_KEY_F))
			std::cout << "TestGame: JUMP is pressed!" << std::endl;

		//if (cotwin::Input::is_mouse_button_pressed(CW_MOUSEBUTTON_LEFT))
		//	std::cout << "TestGame: SHOOT is pressed!" << std::endl;

		cotwin::Vector2 mouse_position = cotwin::Input::get_mouse_position();
		std::cout << "TestGame: Mouse Position (" << mouse_position.x << ", " << mouse_position.y << ")" << std::endl;
	}

	void on_event(cotwin::Event* event) override
	{
		event->processed = true;

		if (event->type == cotwin::ApplicationQuit)
		{
			// for example, save game state to file
			std::cout << "TestGame: Game is stopping! (ApplicationQuit)" << std::endl;
		}
		else if (event->type == cotwin::KeyPress)
		{
			std::cout << "TestGame: " << dynamic_cast<cotwin::KeyboardKeyEvent*>(event)->keyname << " was pressed!" << std::endl;
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

	void on_imgui_render() override
	{
		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&color);		// Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
		show_another_window = false;
		ImGui::End();
		}
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

	system("pause");
	
	return 0;
}