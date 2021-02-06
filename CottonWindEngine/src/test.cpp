//#define CW_MODERN_OPENGL

#include "cottonwind/cottonwind.h"

#include "test/player_script.h"
#include "test/follower_script.h"


// layer where the main game logic and render is
class TestMainLayer : public cotwin::Layer
{
private:
	cotwin::Scene::Entity audio_snap_entity;
	cotwin::Scene::Entity camera_entity;

	cotwin::Scene scene;

	std::vector<glm::ivec4> sensei_animation_frames;

	cotwin::Scene::Entity player_entity;

public:
	TestMainLayer()
		: cotwin::Layer("main")
	{}

	void create_sensei_entity(const glm::ivec2& position)
	{
		static cotwin::Texture& sensei_texture = cotwin::ResourceManager::load_texture("src/test/resources/textures/sensei_running.bmp");

		cotwin::Scene::Entity sensei_entity = scene.create_entity("sensei");
		sensei_entity.add_component<cotwin::TransformComponent>(glm::vec2{ (float)position.x, (float)position.y }, glm::vec2{ 0.0f, 0.0f });
		sensei_entity.add_component<cotwin::SpriteComponent>(
			// here texture_rect is initialized with zeros, since it will be initialized later on by AnimationSystem
			sensei_texture, glm::ivec4{ 0, 0, 0, 0 }, glm::ivec2{ 100, 100 }
		);
		// set up animation for sensei entity
		sensei_entity.add_component<cotwin::AnimationComponent>(0.2f, &sensei_animation_frames);

		sensei_entity.add_component<cotwin::ColliderComponent>(glm::vec2{ 100.0f, 100.0f });
	}

	void create_follower_entity(const glm::ivec2& position)
	{
		static cotwin::Texture& follower_texture = cotwin::ResourceManager::load_texture("src/test/resources/textures/sensei_running.bmp");

		cotwin::Scene::Entity follower_entity = scene.create_entity("follower");
		follower_entity.add_component<cotwin::TransformComponent>(glm::vec2{ (float)position.x, (float)position.y }, glm::vec2{ 0.0f, 0.0f });
		follower_entity.add_component<cotwin::SpriteComponent>(
			// here texture_rect is initialized with zeros, since it will be initialized later on by AnimationSystem
			follower_texture, glm::ivec4{ 0, 0, 0, 0 }, glm::ivec2{ 100, 100 }
		);
		// set up animation for sensei entity
		follower_entity.add_component<cotwin::AnimationComponent>(0.2f, &sensei_animation_frames);

		follower_entity.add_component<cotwin::Scene::ScriptComponent>(new FollowerScript(player_entity), follower_entity);
	}

	virtual void on_attach() override
	{
		cotwin::Logger::Trace("test main layer on attach");
		// for OpenGL Renderer2D test
		//cotwin::Renderer2D::init_render_data();


		// LOAD RESOURCES /////////////////////////////////

		cotwin::Texture& test_texture = cotwin::ResourceManager::load_texture("src/test/resources/textures/test_texture.bmp");
		cotwin::ResourceManager::load_texture("src/test/resources/textures/Hero.bmp");
		cotwin::Audio& snap_audio = cotwin::ResourceManager::load_audio("src/test/resources/audio/snap.wav");
		cotwin::ResourceManager::load_font("src/test/resources/fonts/Lato/Lato-Regular.ttf", 28);


		// CREATE ENTITIES /////////////////////////////////
		
		// Player entity
		player_entity = scene.create_entity("player");

		player_entity.add_component<cotwin::Scene::ScriptComponent>(new PlayerScript(scene), player_entity);

		// set sensei animation frames
		for (int i = 0; i < 11; i++)
		{
			sensei_animation_frames.push_back(glm::ivec4{ i * 24, 0, 24, 24 });
		}

		// Audio entity
		audio_snap_entity = scene.create_entity("snapper");
		audio_snap_entity.add_component<cotwin::AudioEffectComponent>(snap_audio);

		glm::ivec2 window_size = cotwin::Window::get_window_size();
		
		// Camera entity
		camera_entity = scene.create_entity("primary camera");
		camera_entity.add_component<cotwin::TransformComponent>(glm::vec2{ window_size.x / 2.0f, window_size.y / 2.0f }, glm::vec2{ 0.0f, 0.0f });
		camera_entity.add_component<cotwin::CameraComponent>(glm::vec2{ 1280.0f, 720.0f }, window_size);
	}

	virtual void on_detach() override
	{
		cotwin::Logger::Trace("test main layer on detach");
	}

	virtual void on_update(float delta) override
	{
		// for OpenGL Renderer2D test
		//cotwin::Renderer2D::draw_triangle();

		scene.update(delta);
	}

	virtual void on_event(cotwin::Event* event) override
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

		scene.on_window_resize_event(new_size);
	}

	void on_key_press(cotwin::KeyboardEvent* event)
	{
		//cotwin::Logger::Debug("TestGame: %s was pressed!", event->data.key.keyname);

		if (event->data.key.keycode == CW_KEY_G)
		{
			audio_snap_entity.get_component<cotwin::AudioEffectComponent>().play = true;
		}
	}

	void on_mouse_button_press(cotwin::MouseEvent* event)
	{
		cotwin::Logger::Debug("TestGame: %d mouse button was pressed!", event->data.button.button_code);
		if (event->data.button.button_code == CW_MOUSEBUTTON_LEFT)
		{
			audio_snap_entity.get_component<cotwin::AudioEffectComponent>().play = true;
			create_sensei_entity(event->data.button.cursor_position);
		}
		else if (event->data.button.button_code == CW_MOUSEBUTTON_RIGHT)
		{
			create_follower_entity(event->data.button.cursor_position);
		}
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
	virtual void on_attach() override
	{
		color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		show_demo_window = true;
		show_another_window = false;
	}
	virtual void on_update(float delta) override
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

	virtual void on_init() override
	{
		attach_layer(new TestMainLayer());
		//attach_layer(new DubugInfoLayer(static_cast<cotwin::OpenGLGraphics*>(get_graphics())));
		
		glm::u8vec4 clear_color = { 110, 120, 125, 255 };
		cotwin::Renderer2D::set_clear_color(clear_color);
	}

	virtual void on_destroy() override
	{
		cotwin::Logger::Trace("test game destroy");
	}
};



int main(int argc, char* args[])
{
	cotwin::Logger::set_log_priority(cotwin::TracePriority);
	
	//TestGame game(cotwin::WindowProperties("Test Game", 0, 0, 1280, 720, cotwin::Centered | cotwin::Resizable));
	TestGame game(cotwin::WindowProperties("Test Game", 0, 0, 1920, 1080, cotwin::Centered | cotwin::Borderless | cotwin::Vsync));
	
	game.start();
	
	return 0;
}