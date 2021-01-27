#pragma once

// if client project has included SDL_ttf.h, use for SDL2 Text Rendering

#if defined(__has_include)
#if __has_include(<SDL_ttf.h>)
#include <SDL_ttf.h>
#define CW_SDL_TTF_AVAILABLE
#else
// TODO : SDL_ttf.h is unavailable, handle this
#endif
#else
// TODO : SDL_ttf.h is unavailable, handle this
#endif