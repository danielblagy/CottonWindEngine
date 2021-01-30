//#define CW_MODERN_OPENGL

#include "cottonwind/cottonwind.h"


// layer where the main game logic and render is
class TestMainLayer : public cotwin::Layer
{
private:
	cotwin::Entity audio_snap_entity;
	cotwin::Entity camera_entity;

	cotwin::Scene scene;

	cotwin::Text sprites_drawn_label;

	std::vector<glm::ivec4> sensei_animation_frames;

	std::vector<glm::ivec4> player_idle_frames;
	std::vector<glm::ivec4> player_running_right_frames;
	std::vector<glm::ivec4> player_running_left_frames;
	std::vector<glm::ivec4> player_running_down_frames;
	std::vector<glm::ivec4> player_running_up_frames;

	cotwin::Text collision_detected_message;

public:
	TestMainLayer()
		: cotwin::Layer("main")
	{}

	void create_sensei_entity(const glm::ivec2& position)
	{
		static cotwin::Texture& sensei_texture = cotwin::ResourceManager::load_texture("src/test/resources/textures/sensei_running.bmp");

		cotwin::Entity sensei_entity = scene.create_entity("sensei");
		sensei_entity.set<cotwin::TransformComponent>({ glm::vec2{ (float)position.x, (float)position.y }, glm::vec2{ 0.0f, 0.0f } });
		sensei_entity.set<cotwin::SpriteComponent>({
			// here texture_rect is initialized with zeros, since it will be initialized later on by AnimationSystem
			sensei_texture, glm::ivec4{ 0, 0, 0, 0 }, glm::ivec2{ 100, 100 }
		});
		// set up animation for sensei entity
		sensei_entity.set<cotwin::AnimationComponent>({ 0.2f, &sensei_animation_frames });

		sensei_entity.set<cotwin::ColliderComponent>({ glm::vec2{ 100.0f, 100.0f } });
	}

	virtual void on_attach() override
	{
		cotwin::Logger::Trace("test main layer on attach");
		// for OpenGL Renderer2D test
		//cotwin::Renderer2D::init_render_data();


		// LOAD RESOURCES /////////////////////////////////

		cotwin::Texture& test_texture = cotwin::ResourceManager::load_texture("src/test/resources/textures/test_texture.bmp");
		cotwin::Texture& hero_texture = cotwin::ResourceManager::load_texture("src/test/resources/textures/Hero.bmp");
		cotwin::Audio& snap_audio = cotwin::ResourceManager::load_audio("src/test/resources/audio/snap.wav");
		cotwin::Font& main_font = cotwin::ResourceManager::load_font("src/test/resources/fonts/Lato/Lato-Regular.ttf", 28);


		// CREATE ENTITIES /////////////////////////////////
		
		// Player entity
		cotwin::Entity player_entity = scene.create_entity("player");
		player_entity.set<cotwin::TransformComponent>({ glm::vec2{ 700.0f, 500.0f }, glm::vec2{ 0.0f, 0.0f } });
		player_entity.set<cotwin::SpriteComponent>({
			hero_texture, glm::ivec4{ 0, 0, 0, 0 }, glm::ivec2{ 100, 100 }
		});

		player_entity.set<cotwin::AnimationComponent>({ 0.5f, &player_idle_frames });

		// move player with keyboard input
		player_entity.set<cotwin::MovementControlComponent>({
			[](glm::vec2& transform_velocity, float delta) {
				// TODO : or maybe handle delta in TransformSystem ??
				
				if (cotwin::Input::is_key_pressed(CW_KEY_LEFT))
					transform_velocity.x = -150.0f * delta;
				else if (cotwin::Input::is_key_pressed(CW_KEY_RIGHT))
					transform_velocity.x = 150.0f * delta;
				else
					transform_velocity.x = 0.0f;

				if (cotwin::Input::is_key_pressed(CW_KEY_UP))
					transform_velocity.y = -150.0f * delta;
				else if (cotwin::Input::is_key_pressed(CW_KEY_DOWN))
					transform_velocity.y = 150.0f * delta;
				else
					transform_velocity.y = 0.0f;
			}
		});
		// set update logic for the player entity
		player_entity.set<cotwin::ScriptComponent>({
			[&](cotwin::Entity entity, float delta) {
				glm::vec2 velocity = entity.get<cotwin::TransformComponent>()->velocity;
				// get_mut return mutable pointer, if get is used, the pointer will be const, const cotwin::AnimationComponent* in this case
				cotwin::AnimationComponent* animation = entity.get_mut<cotwin::AnimationComponent>();

				// TODO : player animation doesn't update properly

				static glm::vec2 old_velocity = velocity;
				
				if (old_velocity.x != velocity.x || old_velocity.y != velocity.y)
				{
					if (velocity.x > 0.0f)
						animation->set_animation(&player_running_right_frames);
					else if (velocity.x < 0.0f)
						animation->set_animation(&player_running_left_frames);
					else if (velocity.y > 0.0f)
						animation->set_animation(&player_running_down_frames);
					else if (velocity.y < 0.0f)
						animation->set_animation(&player_running_up_frames);
					else
						animation->set_animation(&player_idle_frames);

					old_velocity = velocity;
				}

				// since there is only one player, scene.get_collisions is expected to return just one collision
				// if the player entity & a sensei entity collide, in this case simply display a text on the screen
				/*for (auto& collision : scene.get_collisions("player", "sensei"))
				{
					cotwin::Renderer2D::render_text(collision_detected_message);
				}*/
			}
		});

		player_entity.set<cotwin::ColliderComponent>({ glm::vec2{ 100.0f, 100.0f } });

		// set sensei animation frames
		for (int i = 0; i < 11; i++)
		{
			sensei_animation_frames.push_back(glm::ivec4{ i * 24, 0, 24, 24 });
		}

		player_running_down_frames.reserve(4);
		player_running_up_frames.reserve(4);
		player_running_left_frames.reserve(4);
		player_running_right_frames.reserve(4);
		player_idle_frames.reserve(1);
		
		// set player animation frames
		for (int i = 0; i < 4; i++)
		{
			player_running_down_frames.push_back(glm::ivec4{ i * 256, 0, 256, 256 });
			player_running_up_frames.push_back(glm::ivec4{ i * 256, 256, 256, 256 });
			player_running_left_frames.push_back(glm::ivec4{ i * 256, 2 * 256, 256, 256 });
			player_running_right_frames.push_back(glm::ivec4{ i * 256, 3 * 256, 256, 256 });
		}

		player_idle_frames.push_back(glm::ivec4{ 2 * 256, 0, 256, 256 });

		// Audio entity
		audio_snap_entity = scene.create_entity("snapper");
		audio_snap_entity.set<cotwin::AudioEffectComponent>({ snap_audio });

		// Camera entity
		camera_entity = scene.create_entity("primary camera");
		camera_entity.set<cotwin::TransformComponent>({ glm::vec2{ 1280.0f / 2.0f, 720.0f / 2.0f }, glm::vec2{ 0.0f, 0.0f } });
		// TODO : set up a way to get window size
		camera_entity.set<cotwin::CameraComponent>({ glm::vec2{ 1280, 720 }, glm::vec2{ 1280, 720 } });


		// CREATE TEXTS /////////////////////////////////
		
		// init once, not on each update
		sprites_drawn_label = cotwin::Text(
			"Sprites drawn: ",
			main_font,
			{ 200, 200, 200, 255 },
			{ 0, 0 }
		);

		collision_detected_message = cotwin::Text(
			"Player & sensei collision detected!",
			main_font,
			{ 200, 200, 200, 255 },
			{ 300, 300 }
		);
	}

	virtual void on_detach() override
	{
		cotwin::Logger::Trace("test main layer on detach");
	}

	virtual void on_update(float delta) override
	{
		// for OpenGL Renderer2D test
		//cotwin::Renderer2D::draw_triangle();

		scene.on_update(delta);

		cotwin::Renderer2D::render_text(sprites_drawn_label);
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
			audio_snap_entity.get_mut<cotwin::AudioEffectComponent>()->play = true;
		}
	}

	void on_mouse_button_press(cotwin::MouseEvent* event)
	{
		cotwin::Logger::Debug("TestGame: %d mouse button was pressed!", event->data.button.button_code);
		if (event->data.button.button_code == CW_MOUSEBUTTON_LEFT)
		{
			audio_snap_entity.get_mut<cotwin::AudioEffectComponent>()->play = true;
			create_sensei_entity(event->data.button.cursor_position);
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
		
		//enable_vsync();
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