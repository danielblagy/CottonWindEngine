#pragma once

#ifdef CW_MODERN_OPENGL
// TODO : openglgl fonts & text rendering
#else
#include "sdl2/find_sdl_ttf.h"
#ifdef CW_SDL_TTF_AVAILABLE
#include "sdl2/font.h"
#include "sdl2/font_manager.h"
#include "sdl2/text.h"
#else
// TODO : handle the situation where Moder OpenGL is not used & SDL_ttf.h is not used in client project
#endif
#endif