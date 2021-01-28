//#define CW_MODERN_OPENGL

#include "cottonwind/cottonwind.h"


// layer where the main game logic and render is
class TestMainLayer : public cotwin::Layer
{
private:
	cotwin::Entity* player_entity;
	cotwin::Entity* sensei_entity;
	cotwin::Entity* audio_snap_entity;

	cotwin::Entity* camera_entity;

	cotwin::Scene scene;

public:
	TestMainLayer()
		: cotwin::Layer("main")
	{}

	virtual void on_attach() override
	{
		cotwin::Logger::Trace("test main layer on attach");
		// for OpenGL Renderer2D test
		//cotwin::Renderer2D::init_render_data();

		cotwin::Texture& test_texture = cotwin::ResourceManager::load_texture("src/test/resources/textures/test_texture.bmp");
		cotwin::Texture& sensei_texture = cotwin::ResourceManager::load_texture("src/test/resources/textures/sensei_running.bmp");

		player_entity = scene.create_entity("player");
		player_entity->assign<cotwin::TransformComponent>(glm::vec2{ 700.0f, 500.0f }, glm::vec2{ 0.0f, 0.0f });
		player_entity->assign<cotwin::SpriteComponent>(
			test_texture, glm::ivec4{ 0, 0, test_texture.get_width(), test_texture.get_height() }, glm::ivec2{ 100, 100 }
		);

		sensei_entity = scene.create_entity("sensei");
		sensei_entity->assign<cotwin::TransformComponent>(glm::vec2{ 900.0f, 500.0f }, glm::vec2{ 0.0f, 0.0f });
		sensei_entity->assign<cotwin::SpriteComponent>(
			// here texture_rect is initialized with zeros, since it will be initialized later on by AnimationSystem
			sensei_texture, glm::ivec4{ 0, 0, 0, 0 }, glm::ivec2{ 100, 100 }
		);
		
		cotwin::ComponentHandle<cotwin::AnimationComponent> animation = sensei_entity->assign<cotwin::AnimationComponent>(1.0f);
		for (int i = 0; i < 12; i++)
		{
			animation->frames.push_back(glm::ivec4{ i * 24, 0, 24, 24 });
		}

		// Audio test
		cotwin::Audio& snap_audio = cotwin::ResourceManager::load_audio("src/test/resources/audio/snap.wav");
		
		audio_snap_entity = scene.create_entity("snapper");
		audio_snap_entity->assign<cotwin::AudioEffectComponent>(snap_audio);

		// load font
		cotwin::ResourceManager::load_font("src/test/resources/fonts/Lato/Lato-Regular.ttf", 28);

		camera_entity = scene.create_entity("primary camera");
		camera_entity->assign<cotwin::TransformComponent>(glm::vec2{ 1280.0f / 2.0f, 720.0f / 2.0f }, glm::vec2{ 0.0f, 0.0f });
		// TODO : set up a way to get window size
		camera_entity->assign<cotwin::CameraComponent>(glm::vec2{ 1280, 720 }, glm::vec2{ 1280, 720 });
	}

	virtual void on_detach() override
	{
		cotwin::Logger::Trace("test main layer on detach");
	}

	virtual void on_update(float delta) override
	{
		// for OpenGL Renderer2D test
		//cotwin::Renderer2D::draw_triangle();
		
		if (cotwin::Input::is_key_pressed(CW_KEY_LEFT))
		{
			cotwin::ComponentHandle<cotwin::TransformComponent> transform = player_entity->get<cotwin::TransformComponent>();
			transform.isValid();	// should return true if entity has that component
			transform->center.x -= 1.0f;
		}
		else if (cotwin::Input::is_key_pressed(CW_KEY_RIGHT))
		{
			cotwin::ComponentHandle<cotwin::TransformComponent> transform = player_entity->get<cotwin::TransformComponent>();
			transform.isValid();	// should return true if entity has that component
			transform->center.x += 1.0f;
		}
		
		if (cotwin::Input::is_key_pressed(CW_KEY_UP))
		{
			cotwin::ComponentHandle<cotwin::TransformComponent> transform = player_entity->get<cotwin::TransformComponent>();
			transform.isValid();	// should return true if entity has that component
			transform->center.y -= 1.0f;
		}
		else if (cotwin::Input::is_key_pressed(CW_KEY_DOWN))
		{
			cotwin::ComponentHandle<cotwin::TransformComponent> transform = player_entity->get<cotwin::TransformComponent>();
			transform.isValid();	// should return true if entity has that component
			transform->center.y += 1.0f;
		}

		scene.on_update(delta);

		// for SDL2_ttf test
		cotwin::Renderer2D::render_text(
			"Hello World! This is CottonWind!",
			cotwin::ResourceManager::get_font("src/test/resources/fonts/Lato/Lato-Regular.ttf"),
			{255, 255, 255, 255},
			{ 200, 200 }
		);

		// update fps count every second

		static float seconds_passed = 0.0f;
		static int fps_count = 0;
		static int fps_display = 0;
		seconds_passed += delta;
		fps_count++;
		if (seconds_passed >= 1.0f)
		{
			fps_display = fps_count;
			fps_count = 0;
			seconds_passed -= 1.0f;
		}

		cotwin::Renderer2D::render_text(
			("FPS: " + std::to_string(fps_display)).c_str(),
			cotwin::ResourceManager::get_font("src/test/resources/fonts/Lato/Lato-Regular.ttf"),
			{ 255, 255, 255, 255 },
			{ 800, 300 }
		);
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
		cotwin::Logger::Debug("TestGame: %s was pressed!", event->data.key.keyname);

		if (event->data.key.keycode == CW_KEY_G)
		{
			audio_snap_entity->get<cotwin::AudioEffectComponent>()->play = true;
		}
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
		
		enable_vsync();
		//set_fps_cap(120);
		glm::u8vec4 clear_color = { 120, 70, 150, 255 };
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
	
	TestGame game(cotwin::WindowProperties("Test Game", 0, 0, 1280, 720, cotwin::Centered | cotwin::Resizable));
	
	game.start();

	system("pause");
	
	return 0;
}