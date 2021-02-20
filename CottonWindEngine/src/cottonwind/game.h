#pragma once

#include "events/event.h"
#include "events/keyboard_event.h"
#include "events/mouse_event.h"
#include "events/window_event.h"

#include "layer/layer_stack.h"

#include "util/logger.h"

#include "graphics/graphics.h"
#include "graphics/renderer.h"

//#include "audio/audio.h"


namespace cotwin
{
	class Game
	{
	protected:
		// Game will stop update loop, execute on_destroy() and clean up when running is set to false
		bool running = false;

	private:
		Graphics* graphics;
	
		LayerStack layer_stack;

	
	public:
		Game(WindowProperties window_properties)
		{
			running = init(window_properties);
		}

		virtual ~Game() = default;

		void start()
		{
			on_init();
			
			Uint32 last_time = SDL_GetTicks();
			Uint32 time_passed = 0;
			int fps_count = 0;

			while (running)
			{
				Uint32 current_time = SDL_GetTicks();
				Uint32 ms_passed = current_time - last_time;
				last_time = current_time;

				// time delta in seconds
				float delta = ms_passed / 1000.0f;

				time_passed += ms_passed;

				if (time_passed >= 1000)
				{
					Logger::Debug("fps: %d", fps_count);
					fps_count = 0;
					time_passed -= 1000;
				}

				fps_count++;
				
				handle_sdl_events();

				//Audio::AudioPlayback::play();

				// clear screen
				Renderer2D::clear();
				
				// update and render for each layer from the bottom to the top
				for (Layer* layer : layer_stack)
					layer->on_update(delta);

				// update screen with rendering
				Renderer2D::flush();
			}

			stop();
		}

		void stop()
		{
			on_destroy();
			
			graphics->destroy();
		}

		void attach_layer(Layer* layer)
		{
			layer_stack.push_layer(layer);
			layer->on_attach();
		}

		virtual void on_init() = 0;
		virtual void on_destroy() = 0;

		bool is_running()
		{
			return running;
		}

		void enable_vsync()
		{
			graphics->enable_vsync();
		}

		void disable_vsync()
		{
			graphics->disable_vsync();
		}
		
		Graphics* get_graphics()
		{
			return graphics;
		}

	private:
		bool init(WindowProperties window_properties)
		{
			graphics = new_graphics_instance();
			return graphics->init(&window_properties);
		}

		void on_event(Event* event)
		{
			// process event from the top to the bottom
			for (auto it = layer_stack.rbegin(); it	!= layer_stack.rend(); ++it)
			{
				if (event->processed)
					break;
				
				(*it)->on_event(event);
			}
		}

		void handle_sdl_events()
		{
			SDL_Event e;
			while (SDL_PollEvent(&e))
			{
				if (e.type >= SDL_KEYDOWN && e.type <= SDL_KEYMAPCHANGED)
				{
					EventType type;

					switch (e.type)
					{
					case SDL_KEYDOWN: {
						type = KeyPress;
					} break;
					case SDL_KEYUP: {
						type = KeyRelease;
					} break;
					case SDL_TEXTINPUT: {
						type = TextInput;
					} break;
					default:
						type = Unsupported;
					}

					if (type == KeyPress || type == KeyRelease)
					{
						KeyboardEvent event(type, e.key.keysym.scancode, SDL_GetScancodeName(e.key.keysym.scancode), e.key.repeat);
						on_event(&event);
					}
					else if (type == TextInput)
					{
						KeyboardEvent event(e.text.text);
						on_event(&event);
					}
					else
					{
						Event event;
						event.category = EventCategoryNone;
						event.type = type;
						on_event(&event);
					}
				}
				else if (e.type >= SDL_MOUSEMOTION && e.type <= SDL_MOUSEWHEEL)
				{
					switch (e.type)
					{
					case SDL_MOUSEMOTION: {
						MouseEvent event({ e.motion.x, e.motion.y }, { e.motion.xrel, e.motion.yrel });
						on_event(&event);
					} break;
					case SDL_MOUSEBUTTONDOWN: {
						MouseEvent event(MouseButtonPress, { e.motion.x, e.motion.y }, e.button.button, e.button.clicks == 2);
						on_event(&event);
					} break;
					case SDL_MOUSEBUTTONUP: {
						MouseEvent event(MouseButtonRelease, { e.motion.x, e.motion.y }, e.button.button, e.button.clicks == 2);
						on_event(&event);
					} break;
					case SDL_MOUSEWHEEL: {
						MouseEvent event({ e.wheel.x, e.wheel.y });
						on_event(&event);
					} break;
					default: {
						Event event;
						event.category = EventCategoryNone;
						event.type = Unsupported;
						on_event(&event);
					}
					}
				}
				else if (e.type == SDL_QUIT)
				{
					Event event;
					event.category = EventCategoryWindow;
					event.type = ApplicationQuit;
					on_event(&event);

					// stop the game
					running = false;
				}
				else if (e.type == SDL_WINDOWEVENT)
				{
					switch (e.window.event)
					{
					case SDL_WINDOWEVENT_CLOSE: {
						WindowEvent event(WindowClose);
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_MINIMIZED: {
						WindowEvent event(WindowMinimize);
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_MAXIMIZED: {
						WindowEvent event(WindowMaximize);
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_FOCUS_GAINED: {
						WindowEvent event(WindowFocusGained);
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_FOCUS_LOST: {
						WindowEvent event(WindowFocusLost);
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_MOVED: {
						WindowEvent event(WindowMove, { (int)e.window.data1, (int)e.window.data2 });
						on_event(&event);
					} break;
					case SDL_WINDOWEVENT_RESIZED: {
						WindowEvent event(WindowResize, { (int)e.window.data1, (int)e.window.data2 });
						on_event(&event);
					} break;
					default: {
						Event event;
						event.category = EventCategoryNone;
						event.type = Unsupported;
						on_event(&event);
					}
					}
				}
			}
		}
	};
}