#include "../cottonwind/cottonwind.h"

#include "script.h"


class MainLayer : public cotwin::Layer
{
private:
	cotwin::Scene scene;
	std::vector<glm::ivec4> flame_animation;

public:
	MainLayer()
		: cotwin::Layer("main")
	{}
	
	virtual void on_attach() override
	{
		cotwin::random::init();
		
		// create the player
		cotwin::Scene::Entity player = scene.create_entity("player");
		player.add_component<cotwin::Scene::ScriptComponent>(new PlayerScript(), player);

		// create camera
		cotwin::Scene::Entity camera = scene.create_entity("camera");
		camera.add_component<cotwin::TransformComponent>(glm::vec2{ 350.0f, 450.0f }, glm::vec2{ 0.0f, 0.0f });
		camera.add_component<cotwin::CameraComponent>(glm::vec2{ 700.0f, 900.0f }, glm::ivec2{ 700.0f, 900.0f }, true);

		create_boundary("death boundary", { 50.0f, 0.0f, 650.0f, 50.0f });
		create_boundary("death boundary", { 50.0f, 850.0f, 650.0f, 50.0f });
		create_boundary("goal boundary", { 0.0f, 0.0f, 50.0f, 900.0f });
		create_boundary("goal boundary", { 650.0f, 0.0f, 50.0f, 900.0f });

		// flames
		for (int i = 0; i < 8; i++)
			flame_animation.push_back(glm::ivec4{ 256 * i, 0, 256, 256 });
		
		const int flame_w = 80.0f, flame_h = 80.0f;

		static cotwin::Texture& texture = cotwin::ResourceManager::load_texture("src/bird_game/resources/flame.bmp");
		
		for (int i = 0; i < 10; i++)
		{
			cotwin::Scene::Entity flame_left = scene.create_entity("left flame");

			flame_left.add_component<cotwin::TransformComponent>(glm::vec2{ 40.0f, 50.0f + flame_h * i * 2.0f }, glm::vec2{ 0.0f, 0.0f });
			flame_left.add_component<cotwin::SpriteComponent>(
				texture, glm::ivec4{ 0, 0, 0, 0 }, glm::ivec2{ flame_w * 2.0f, flame_h * 2.0f }
			);
			flame_left.add_component<cotwin::AnimationComponent>(0.2f, &flame_animation);


			flame_left.add_component<cotwin::ColliderComponent>(
				glm::vec2{ flame_w, flame_h },
				glm::vec2{ flame_w / 2.0f, flame_w / 2.0f }
			);

			flame_left.add_component<cotwin::Scene::ScriptComponent>(new FlameScript(player), flame_left);


			cotwin::Scene::Entity flame_right = scene.create_entity("right flame");

			flame_right.add_component<cotwin::TransformComponent>(glm::vec2{ 520.0f, 50.0f + flame_h * i * 2.0f }, glm::vec2{ 0.0f, 0.0f });
			flame_right.add_component<cotwin::SpriteComponent>(
				texture, glm::ivec4{ 0, 0, 0, 0 }, glm::ivec2{ flame_w * 2.0f, flame_h * 2.0f }
			);
			flame_right.add_component<cotwin::AnimationComponent>(0.2f, &flame_animation);


			flame_right.add_component<cotwin::ColliderComponent>(
				glm::vec2{ flame_w, flame_h },
				glm::vec2{ flame_w / 2.0f, flame_w / 2.0f }
			);

			flame_right.add_component<cotwin::Scene::ScriptComponent>(new FlameScript(player), flame_right);
		}
	}

	virtual void on_update(float delta) override
	{
		scene.update(delta);
	}

	virtual void on_event(cotwin::Event* event) override
	{}

private:
	void create_boundary(const char* tag, const glm::vec4& rect)
	{
		static cotwin::Texture& texture = cotwin::ResourceManager::load_texture("src/bird_game/resources/rect.bmp");
		
		cotwin::Scene::Entity boundary = scene.create_entity(tag);

		boundary.add_component<cotwin::TransformComponent>(glm::vec2{ rect.x, rect.y }, glm::vec2{ 0.0f, 0.0f });
		boundary.add_component<cotwin::SpriteComponent>(
			texture, glm::ivec4{ 0, 0, texture.get_width(), texture.get_height() }, glm::ivec2{ rect.z, rect.w }
		);
		
		boundary.add_component<cotwin::ColliderComponent>(glm::vec2{ rect.z, rect.w });
	}
};


class BirdGame : public cotwin::Game
{
public:
	BirdGame(cotwin::WindowProperties window_properties)
		: Game(window_properties)
	{}
	
	virtual void on_init() override
	{
		attach_layer(new MainLayer());

		cotwin::Renderer2D::set_clear_color(glm::u8vec4 { 177, 240, 199, 255 });
	}
};


int main(int argc, char* args[])
{
	BirdGame game(cotwin::WindowProperties("The Bird Game", 0, 0, 700, 900, cotwin::Centered | cotwin::Resizable | cotwin::Vsync));
	game.start();
	
	return 0;
}