#pragma once

#include <stdio.h>


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
				printf("CRITICAL: ");
				printf(message, args...);
				printf("\n");
			}
		}
	};

	LogPriority Logger::priority = InfoPriority;
}