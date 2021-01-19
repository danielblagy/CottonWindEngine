#pragma once

#include <SDL.h>

#define CW_LOG_PRIORITY_TRACE		SDL_LOG_PRIORITY_VERBOSE
#define CW_LOG_PRIORITY_DEBUG		SDL_LOG_PRIORITY_DEBUG
#define CW_LOG_PRIORITY_INFO		SDL_LOG_PRIORITY_INFO
#define CW_LOG_PRIORITY_WARN		SDL_LOG_PRIORITY_WARN
#define CW_LOG_PRIORITY_ERROR		SDL_LOG_PRIORITY_ERROR
#define CW_LOG_PRIORITY_CRITICAL	SDL_LOG_PRIORITY_CRITICAL


namespace cotwin
{
	typedef SDL_LogPriority LogPriority;

	inline void SetLogPriority(LogPriority priority)
	{
		SDL_LogSetAllPriority(priority);
	}
	
	template<typename... Args>
	inline void LogTrace(const char* message, Args... args)
	{
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, message, args...);
	}

	template<typename... Args>
	inline void LogDebug(const char* message, Args... args)
	{
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, message, args...);
	}
	
	template<typename... Args>
	inline void LogInfo(const char* message, Args... args)
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, message, args...);
	}

	template<typename... Args>
	inline void LogWarn(const char* message, Args... args)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, message, args...);
	}

	template<typename... Args>
	inline void LogError(const char* message, Args... args)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, message, args...);
	}

	template<typename... Args>
	inline void LogCritical(const char* message, Args... args)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, args...);
	}
}