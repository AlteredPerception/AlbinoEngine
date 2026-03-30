#include "Logger.h"
#include <iomanip>
#include <sstream>

namespace AlbinoEngine
{
	void Logger::Init(std::string_view filePath, LogLevel minLevel, size_t ringBufferSize) {
		std::scoped_lock lock(s_mtx);

		s_minLevel = minLevel;
		s_ringMax = ringBufferSize;

		// Append so you don't lose previous logs every run
		s_file.open(std::string(filePath), std::ios::out | std::ios::app);
		s_initialized = s_file.is_open();
	}

	void Logger::Shutdown() {
		std::scoped_lock lock(s_mtx);
		if (s_file.is_open()) s_file.close();
		s_initialized = false;
	}

	const char* Logger::ToString(LogLevel lvl) {
		switch (lvl) {
		case LogLevel::Trace: return "TRACE";
		case LogLevel::Info: return "INFO";
		case LogLevel::Warn: return "WARN";
		case LogLevel::Error: return "ERROR";
		case LogLevel::Fatal: return "FATAL";
		default:			  return "UNKNOWN";
		}
	}

	std::string Logger::Format(const LogRecord& r){
		// Timestamp formatting (simple + readable)
		auto tt = std::chrono::system_clock::to_time_t(r.time);
		std::tm tm{};
#if defined(_WIN32)
		localtime_s(&tm, &tt);
#else
		localtime_r(&tt, &tm);
#endif

		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
		oss << " [" << ToString(r.level) << "]";
		oss << " (" << r.file << ":" << r.line << ")";
		oss << " " << r.message;

		return oss.str();
	}

	void Logger::Write(const LogRecord& r) {
		std::string line = Format(r);

		std::scoped_lock lock(s_mtx);

		// ring buffer for UI/debug
		s_ring.push_back(line);
		if (s_ring.size() > s_ringMax) s_ring.pop_front();

		// write to file immediately.
		if (s_file.is_open()) {
			s_file << line << "\n";
			// flush on errors so crashes still keep logs
			if (r.level >= LogLevel::Error) s_file.flush();
		}
	}

	std::string Logger::GetRecentText() {
		std::scoped_lock lock(s_mtx);
		std::string out;
		for (auto& s : s_ring) {
			out += s;
			out += "\n";
		}
		return out;
	}
} // namespace AlbinoEngine