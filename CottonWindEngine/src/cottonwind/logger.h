#pragma once

#include <SDL.h>


namespace cotwin
{
	inline void LogTrace(const char* message)
	{
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, message);
	}

	inline void LogDebug(const char* message)
	{
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, message);
	}
	
	inline void LogInfo(const char* message)
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, message);
	}

	inline void LogWarn(const char* message)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, message);
	}

	inline void LogError(const char* message)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, message);
	}

	inline void LogCritical(const char* message)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, message);
	}
}