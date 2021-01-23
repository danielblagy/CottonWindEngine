#include <iostream>

#define CW_DEBUG_MODE_ENABLED 1
#define CW_GRAPHICS_OPENGL

#include "cottonwind/game.h"
#include "cottonwind/input/input.h"
#include "cottonwind/input/keycodes.h"
#include "cottonwind/input/mouse_buttons.h"
#include "cottonwind/events/event_dispatcher.h"


// layer where the main game logic and render is
class TestMainLayer : public cotwin::Layer
{
private:
	cotwin::Vector4u8 orange_color = { 255, 165, 0 };
	cotwin::Vector4u8 yellow_color = { 255, 255, 0 };

public:
	TestMainLayer()
		: cotwin::Layer("main")
	{}

	void on_attach() override
	{
		std::cout << "test main layer on attach" << std::endl;
		// for OpenGL Renderer2D test
		cotwin::Renderer2D::init_render_data();
	}

	void on_detach() override
	{
		std::cout << "test main layer on detach" << std::endl;
	}

	void on_update(double delta) override
	{
		// for SDL Renderer2D test
		//cotwin::Renderer2D::draw_rect({ 50, 50, 50, 50 }, orange_color);
		//cotwin::Renderer2D::fill_rect({ 500, 450, 120, 60 }, yellow_color);
		
		// for OpenGL Renderer2D test
		cotwin::Renderer2D::draw_triangle();
		
		if (cotwin::Input::is_key_pressed(CW_KEY_F))
			std::cout << "TestGame: JUMP is pressed!" << std::endl;

		//if (cotwin::Input::is_mouse_button_pressed(CW_MOUSEBUTTON_LEFT))
		//	std::cout << "TestGame: SHOOT is pressed!" << std::endl;

		//cotwin::Vector2i mouse_position = cotwin::Input::get_mouse_position();
		//std::cout << "TestGame: Mouse Position (" << mouse_position.x << ", " << mouse_position.y << ")" << std::endl;
	}

	void on_event(cotwin::Event* event) override
	{
		event->processed = true;

		if (event->type == cotwin::Unsupported)
		{
			event->processed = false;
		}
		else
		{
			cotwin::EventDispatcher::dispatch(this, event, cotwin::ApplicationQuit, &TestMainLayer::on_app_quit);
			cotwin::EventDispatcher::dispatch(this, event, cotwin::WindowMove, &TestMainLayer::on_window_move);
			cotwin::EventDispatcher::dispatch(this, event, cotwin::WindowResize, &TestMainLayer::on_window_resize);
			cotwin::EventDispatcher::dispatch(this, event, cotwin::KeyPress, &TestMainLayer::on_key_press);
			cotwin::EventDispatcher::dispatch(this, event, cotwin::MouseButtonPress, &TestMainLayer::on_mouse_button_press);
		}
	}

	void on_app_quit(cotwin::WindowEvent* event)
	{
		// for example, save game state to file
		std::cout << "TestGame: Game is stopping! (ApplicationQuit)" << std::endl;
	}

	void on_window_move(cotwin::WindowEvent* event)
	{
		cotwin::Vector2i new_position = event->data.new_position;
		std::cout << "TestGame: window move (" << new_position.x << ", " << new_position.y << ")" << std::endl;
	}

	void on_window_resize(cotwin::WindowEvent* event)
	{
		cotwin::Vector2i new_size = event->data.new_size;
		std::cout << "TestGame: window resize (" << new_size.x << ", " << new_size.y << ")" << std::endl;
	}

	void on_key_press(cotwin::KeyboardEvent* event)
	{
		std::cout << "TestGame: " << event->data.key.keyname << " was pressed!" << std::endl;
	}

	void on_mouse_button_press(cotwin::MouseEvent* event)
	{
		std::cout << "TestGame: " << event->data.button.button_code << " mouse button was pressed!" << std::endl;
	}

};

class DubugInfoLayer : public cotwin::ImGuiLayer
{
private:
	// gui state
	ImVec4 color;
	bool show_demo_window;
	bool show_another_window;

public:
	DubugInfoLayer(cotwin::OpenGLGraphics* graphics)
		: ImGuiLayer(graphics)
	{}

	void on_attach() override
	{
		color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		show_demo_window = true;
		show_another_window = false;
	}

	void on_update(double delta) override
	{
		new_imgui_frame();

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

		render_imgui_frame();
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
		attach_layer(new TestMainLayer());
		
		//cotwin::OpenGLGraphics* graphics = dynamic_cast<cotwin::OpenGLGraphics*>(get_graphics());
		//attach_layer(new DubugInfoLayer(graphics));

		enable_vsync();
		cotwin::Vector4u8 clear_color = { 120, 70, 150, 255 };
		set_render_clear_color(clear_color);
	}

	void on_destroy() override
	{
		std::cout << "test game destroy" << std::endl;
	}
};

int main(int argc, char* args[])
{
	//cotwin::SetLogPriority(CW_LOG_PRIORITY_TRACE);
	TestGame game(cotwin::WindowProperties("Test Game", 0, 0, 1280, 720, cotwin::Centered | cotwin::Resizable));
	//TestGame game(cotwin::WindowProperties("Test Game", 50, 250, 1280, 720, cotwin::Borderless));
	//TestGame game(cotwin::WindowProperties("Test Game", 0, 0, 1280, 720, cotwin::Fullscreen));
	
	game.start();

	system("pause");
	
	return 0;
}