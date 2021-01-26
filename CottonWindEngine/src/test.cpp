//#define CW_MODERN_OPENGL

#include "cottonwind/cottonwind.h"


// layer where the main game logic and render is
class TestMainLayer : public cotwin::Layer
{
private:
	glm::u8vec4 orange_color = { 255, 165, 0, 255 };
	glm::u8vec4 yellow_color = { 255, 255, 0, 255 };
	glm::u8vec4 gray_color = { 50, 50, 50, 255 };

	glm::ivec2 A_point = { 200, 600 };
	glm::ivec2 B_point = { 800, 300 };

	cotwin::Texture test_texture;
	cotwin::Sprite player_sprite;
	
	cotwin::Texture sensei_running_texture;
	cotwin::Sprite sensei_sprite;
	cotwin::Animation* sensei_animation;

public:
	TestMainLayer()
		: cotwin::Layer("main")
	{}

	void on_attach() override
	{
		cotwin::Logger::Trace("test main layer on attach");
		// for OpenGL Renderer2D test
		//cotwin::Renderer2D::init_render_data();

		test_texture = cotwin::ResourceManager::load_texture("src/test/resources/textures/test_texture.bmp");
		sensei_running_texture = cotwin::ResourceManager::load_texture("src/test/resources/textures/sensei_running.bmp");

		player_sprite.texture = test_texture;
		player_sprite.texture_rect = { 0, 0, player_sprite.texture.get_width(), player_sprite.texture.get_height() };
		player_sprite.rect = { 700, 500, 100, 100 };

		sensei_sprite.texture = sensei_running_texture;
		sensei_sprite.rect = { 800, 500, 100, 100 };

		sensei_animation = new cotwin::Animation(sensei_sprite, 1.0f);
		for (int i = 0; i < 12; i++)
		{
			sensei_animation->add_frame(glm::ivec4{ i * 24, 0, 24, 24 });
		}
		sensei_animation->refresh();
	}

	void on_detach() override
	{
		cotwin::Logger::Trace("test main layer on detach");
	}

	void on_update(float delta) override
	{
		// for SDL Renderer2D test
		cotwin::Renderer2D::draw_rect({ 50, 50, 50, 50 }, orange_color);
		cotwin::Renderer2D::fill_rect({ 500, 450, 120, 60 }, yellow_color);
		cotwin::Renderer2D::draw_line(A_point, B_point, gray_color);
		cotwin::Renderer2D::draw_point(20, 650, {255,0,0,255});

		cotwin::Renderer2D::render_texture(test_texture, { 200, 200, test_texture.get_width(), test_texture.get_height() });

		cotwin::Renderer2D::render_sprite(player_sprite);

		cotwin::Logger::Debug("%d  %d  %d  %d",
			sensei_sprite.texture_rect.x, sensei_sprite.texture_rect.y, sensei_sprite.texture_rect.z, sensei_sprite.texture_rect.w);
		sensei_animation->update(delta);
		cotwin::Renderer2D::render_sprite(sensei_sprite);
		
		// for OpenGL Renderer2D test
		//cotwin::Renderer2D::draw_triangle();
		
		if (cotwin::Input::is_key_pressed(CW_KEY_F))
			cotwin::Logger::Debug("TestGame: JUMP is pressed!");

		if (cotwin::Input::is_key_pressed(CW_KEY_LEFT))
			player_sprite.rect.x -= 1;
		else if (cotwin::Input::is_key_pressed(CW_KEY_RIGHT))
			player_sprite.rect.x += 1;
		
		if (cotwin::Input::is_key_pressed(CW_KEY_UP))
			player_sprite.rect.y -= 1;
		else if (cotwin::Input::is_key_pressed(CW_KEY_DOWN))
			player_sprite.rect.y += 1;

		//if (cotwin::Input::is_mouse_button_pressed(CW_MOUSEBUTTON_LEFT))
			//cotwin::Logger::Debug("TestGame: SHOOT is pressed!");

		//cotwin::Vector2i mouse_position = cotwin::Input::get_mouse_position();
		//cotwin::Logger::Debug("TestGame: Mouse Position (%d, %d)", mouse_position.x, mouse_position.y);;
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
			cotwin::EventDispatcher::dispatch(this, event, cotwin::WindowFocusGained, &TestMainLayer::on_window_focus);
			cotwin::EventDispatcher::dispatch(this, event, cotwin::WindowFocusLost, &TestMainLayer::on_window_unfocus);
		}
	}

	void on_app_quit(cotwin::WindowEvent* event)
	{
		// for example, save game state to file
		cotwin::Logger::Debug("TestGame: Game is stopping! (ApplicationQuit)");
	}

	void on_window_move(cotwin::WindowEvent* event)
	{
		glm::ivec2 new_position = event->data.new_position;
		cotwin::Logger::Debug("TestGame: window move (%d, %d)", new_position.x, new_position.y);
	}

	void on_window_resize(cotwin::WindowEvent* event)
	{
		glm::ivec2 new_size = event->data.new_size;
		cotwin::Logger::Debug("TestGame: window resize (%d, %d)", new_size.x, new_size.y);
	}

	void on_key_press(cotwin::KeyboardEvent* event)
	{
		cotwin::Logger::Debug("TestGame: %s was pressed!", event->data.key.keyname);
	}

	void on_mouse_button_press(cotwin::MouseEvent* event)
	{
		cotwin::Logger::Debug("TestGame: %d mouse button was pressed!", event->data.button.button_code);
	}

	void on_window_focus(cotwin::WindowEvent* event)
	{
		cotwin::Logger::Debug("TestGame: window focus gained");
	}

	void on_window_unfocus(cotwin::WindowEvent* event)
	{
		cotwin::Logger::Debug("TestGame: window focus lost");
	}

};

/*class DubugInfoLayer : public cotwin::ImGuiLayer
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

	void on_update(float delta) override
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
};*/

class TestGame : public cotwin::Game
{
public:
	TestGame(cotwin::WindowProperties window_properties)
		: Game(window_properties)
	{
		cotwin::Logger::Trace("test game constructor");
	}

	void on_init() override
	{
		attach_layer(new TestMainLayer());
		
		//cotwin::OpenGLGraphics* graphics = dynamic_cast<cotwin::OpenGLGraphics*>(get_graphics());
		//attach_layer(new DubugInfoLayer(graphics));

		enable_vsync();
		glm::u8vec4 clear_color = { 120, 70, 150, 255 };
		set_render_clear_color(clear_color);
	}

	void on_destroy() override
	{
		cotwin::Logger::Trace("test game destroy");
	}
};

int main(int argc, char* args[])
{
	cotwin::Logger::set_log_priority(cotwin::TracePriority);
	
	TestGame game(cotwin::WindowProperties("Test Game", 0, 0, 1280, 720, cotwin::Centered | cotwin::Resizable));
	//TestGame game(cotwin::WindowProperties("Test Game", 50, 250, 1280, 720, cotwin::Borderless));
	//TestGame game(cotwin::WindowProperties("Test Game", 0, 0, 1280, 720, cotwin::Fullscreen));
	
	game.start();

	system("pause");
	
	return 0;
}