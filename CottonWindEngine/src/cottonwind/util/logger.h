#pragma once

#include <stdio.h>
#include <mutex>


namespace cotwin
{
	enum LogPriority
	{
		TracePriority, DebugPriority, InfoPriority, WarnPriority, ErrorPriority, CriticalPriority
	};
	
	class Logger
	{
	protected:
		static LogPriority priority;
		static std::mutex log_mutex;

	public:
		static inline void set_log_priority(LogPriority s_priority)
		{
			priority = s_priority;
		}

		template<typename... Args>
		static inline void Trace(const char* message, Args... args)
		{
			if (priority <= TracePriority)
			{
				std::scoped_lock lock(log_mutex);
				printf("TRACE: ");
				printf(message, args...);
				printf("\n");
			}
		}

		template<typename... Args>
		static inline void Debug(const char* message, Args... args)
		{
			if (priority <= DebugPriority)
			{
				std::scoped_lock lock(log_mutex);
				printf("DEBUG: ");
				printf(message, args...);
				printf("\n");
			}
		}

		template<typename... Args>
		static inline void Info(const char* message, Args... args)
		{
			if (priority <= InfoPriority)
			{
				std::scoped_lock lock(log_mutex);
				printf("INFO: ");
				printf(message, args...);
				printf("\n");
			}
		}

		template<typename... Args>
		static inline void Warn(const char* message, Args... args)
		{
			if (priority <= WarnPriority)
			{
				std::scoped_lock lock(log_mutex);
				printf("WARN: ");
				printf(message, args...);
				printf("\n");
			}
		}

		template<typename... Args>
		static inline void Error(const char* message, Args... args)
		{
			if (priority <= ErrorPriority)
			{
				std::scoped_lock lock(log_mutex);
				printf("ERROR: ");
				printf(message, args...);
				printf("\n");
			}
		}

		template<typename... Args>
		static inline void Critical(const char* message, Args... args)
		{
			if (priority <= CriticalPriority)
			{
				std::scoped_lock lock(log_mutex);
				printf("CRITICAL: ");
				printf(message, args...);
				printf("\n");
			}
		}
	};

	LogPriority Logger::priority = InfoPriority;
	std::mutex Logger::log_mutex;
}