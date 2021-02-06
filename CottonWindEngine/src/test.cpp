#include "test/sdl2_test_layer.h"
//#include "test/opengl_test_layer.h"


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
	//TestGame game(cotwin::WindowProperties("Test Game", 0, 0, 1920, 1080, cotwin::Centered | cotwin::Borderless));

	glm::ivec2 window_size = cotwin::Window::get_window_size();
	cotwin::Logger::Info("Window info:\n\tTitle: %s\n\tDimensions: %d, %d", cotwin::Window::get_window_title(), window_size.x, window_size.y);
	
	game.start();
	
	return 0;
}