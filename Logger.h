#pragma once
#include <string>
#include <string_view>
#include <fstream>
#include <mutex>
#include <deque>
#include <chrono>
#include <format>
#include <thread>

namespace AlbinoEngine
{
	enum class LogLevel : int {
		Trace = 0,
		Info,
		Warn,
		Error,
		Fatal
	};

	struct LogRecord {
		std::chrono::system_clock::time_point time;
		LogLevel level;
		std::string message;
		const char* file;
		int line;
		std::thread::id tid;
	};

	class Logger {
	public:
		// Call once at startup
		static void Init(
			std::string_view filePade = "logfile.log",
			LogLevel minLevel = LogLevel::Trace,
			size_t ringBufferSize = 256);

		// Call at shutdown (optional)
		static void Shutdown();

		// Logging API
		template <class... Args>
		static void Log(LogLevel lvl, const char* file, int line,
			std::format_string<Args...> fmt, Args&&... args)
		{
			if (!s_initialized || lvl < s_minLevel) return;

			LogRecord r;
			r.time = std::chrono::system_clock::now();
			r.level = lvl;
			r.message = std::format(fmt, std::forward<Args>(args)...);
			r.file = file;
			r.line = line;
			r.tid = std::this_thread::get_id();

			Write(r);
		}

		// For UI overlay / debugging: last N formatted lines.
		static std::string GetRecentText();
	private:
		static void Write(const LogRecord& r);
		static std::string Format(const LogRecord& r);
		static const char* ToString(LogLevel lvl);

	private:
		static inline std::mutex s_mtx;
		static inline std::ofstream s_file;
		static inline std::deque<std::string> s_ring;
		static inline size_t s_ringMax = 256;
		static inline LogLevel s_minLevel = LogLevel::Trace;
		static inline bool s_initialized = false;

	};
}

// Convenience macros (captures file/line)
#define AL_LOG_INFO(fmt,...) ::AlbinoEngine::Logger::Log(::AlbinoEngine::LogLevel::Info, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define AL_LOG_WARN(fmt,...) ::AlbinoEngine::Logger::Log(::AlbinoEngine::LogLevel::Warn, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define AL_LOG_ERROR(fmt,...) ::AlbinoEngine::Logger::Log(::AlbinoEngine::LogLevel::Error, __FILE__, __LINE__, fmt, __VA_ARGS__)
