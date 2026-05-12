#pragma once

#include "Core.hpp"
#include "Status.hpp"

#include <chrono>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <string>
#include <utility>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Log
{
	/// Severity levels. Trace is the noisiest (per-fire), Error the rarest (unrecoverable).
	enum class Level : int
	{
		Trace = 0,
		Debug = 1,
		Info  = 2,
		Warn  = 3,
		Error = 4,
	};

	/// Bitmask of active log destinations. Combine with operator|.
	enum class Sinks : int
	{
		None    = 0,
		Console = 1 << 0,
		File    = 1 << 1,
	};

	constexpr Sinks operator|(Sinks a, Sinks b) noexcept
	{
		return static_cast<Sinks>(static_cast<int>(a) | static_cast<int>(b));
	}

	constexpr Sinks operator&(Sinks a, Sinks b) noexcept
	{
		return static_cast<Sinks>(static_cast<int>(a) & static_cast<int>(b));
	}

	constexpr bool HasSink(Sinks all, Sinks one) noexcept
	{
		return (static_cast<int>(all) & static_cast<int>(one)) != 0;
	}

	namespace Internal
	{
		// Maximum formatted message length. Messages exceeding this are truncated.
		// Chosen to fit a typical stack buffer while covering >99% of MMAPI log lines.
		inline constexpr size_t kMessageBufferSize = 1024;

		inline Level                  current_level   = Level::Info;
		inline Sinks                  current_sinks   = Sinks::Console;
		inline std::filesystem::path  file_path;
		inline std::ofstream          file_stream;
		inline bool                   file_path_initialized = false;

		constexpr YYTK::CmColor LevelColor(Level level) noexcept
		{
			switch (level)
			{
				case Level::Trace: return YYTK::CM_WHITE;
				case Level::Debug: return YYTK::CM_AQUA;
				case Level::Info:  return YYTK::CM_LIGHTGREEN;
				case Level::Warn:  return YYTK::CM_LIGHTYELLOW;
				case Level::Error: return YYTK::CM_LIGHTRED;
			}
			return YYTK::CM_WHITE;
		}

		constexpr const char* LevelTag(Level level) noexcept
		{
			switch (level)
			{
				case Level::Trace: return "TRACE";
				case Level::Debug: return "DEBUG";
				case Level::Info:  return "INFO ";
				case Level::Warn:  return "WARN ";
				case Level::Error: return "ERROR";
			}
			return "?????";
		}

		inline std::filesystem::path DefaultFilePath(const std::string& mod_name)
		{
			return std::filesystem::current_path()
				/ "mod_data"
				/ mod_name
				/ "logs"
				/ ("MMAPI-" + mod_name + ".log");
		}

		inline void EnsureFilePathInitialized()
		{
			if (file_path_initialized)
				return;

			if (MMAPI::Internal::mod_name.empty())
				return;

			file_path             = DefaultFilePath(MMAPI::Internal::mod_name);
			file_path_initialized = true;
		}

		inline void WriteTimestampedLine(Level level, const char* message)
		{
			EnsureFilePathInitialized();

			if (!file_stream.is_open() && !file_path.empty())
			{
				std::error_code ec;
				std::filesystem::create_directories(file_path.parent_path(), ec);
				// Truncate on first write each session, matching YYTK's log behavior.
				// Subsequent writes append to the same open stream.
				file_stream.open(file_path, std::ios::out | std::ios::trunc);
			}

			if (!file_stream.is_open())
				return;

			using namespace std::chrono;
			auto now  = system_clock::now();
			auto ms   = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
			std::time_t t = system_clock::to_time_t(now);
			std::tm tm{};
			localtime_s(&tm, &t);

			char timestamp[32];
			std::snprintf(timestamp, sizeof(timestamp),
				"%04d-%02d-%02d %02d:%02d:%02d.%03lld",
				tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
				tm.tm_hour, tm.tm_min, tm.tm_sec,
				static_cast<long long>(ms.count()));

			file_stream << timestamp << " [" << LevelTag(level) << "] " << message << '\n';
			file_stream.flush();
		}

		inline void Dispatch(Level level, const char* message)
		{
			if (HasSink(current_sinks, Sinks::Console) && MMAPI::Internal::module_interface)
			{
				MMAPI::Internal::module_interface->Print(
					LevelColor(level),
					"[%s %s] [%s] %s",
					MMAPI::Internal::mod_name.c_str(),
					MMAPI::Internal::mod_version.c_str(),
					LevelTag(level),
					message
				);
			}

			if (HasSink(current_sinks, Sinks::File))
				WriteTimestampedLine(level, message);
		}

		/// Formats the message into a stack buffer and dispatches to active sinks.
		/// If snprintf would write more than kMessageBufferSize bytes, the message is truncated
		/// and "..." is written into the last three bytes before the null terminator so readers
		/// know the line is incomplete.
		template <typename... Args>
		inline void FormatAndDispatch(Level level, const char* format, Args... args)
		{
			if (static_cast<int>(level) < static_cast<int>(current_level))
				return;

			char buffer[kMessageBufferSize];
			int written = std::snprintf(buffer, sizeof(buffer), format, args...);
			if (written > 0 && static_cast<size_t>(written) >= sizeof(buffer))
			{
				buffer[sizeof(buffer) - 4] = '.';
				buffer[sizeof(buffer) - 3] = '.';
				buffer[sizeof(buffer) - 2] = '.';
				buffer[sizeof(buffer) - 1] = '\0';
			}

			Dispatch(level, buffer);
		}
	}

	/// Sets the minimum log level. Messages below this level are dropped before formatting.
	inline void SetLevel(Level level) noexcept { Internal::current_level = level; }
	inline Level GetLevel() noexcept { return Internal::current_level; }

	/// Sets the active log sinks (bitmask). Defaults to Console.
	inline void SetSinks(Sinks sinks) noexcept { Internal::current_sinks = sinks; }
	inline Sinks GetSinks() noexcept { return Internal::current_sinks; }

	/// Overrides the file sink path. Default path is `<cwd>/mod_data/<mod_name>/logs/MMAPI-<mod_name>.log`.
	inline void SetFilePath(std::filesystem::path path)
	{
		if (Internal::file_stream.is_open())
			Internal::file_stream.close();
		Internal::file_path             = std::move(path);
		Internal::file_path_initialized = true;
	}

	/// Returns the active file sink path. Resolves the default lazily on first access.
	inline const std::filesystem::path& GetFilePath()
	{
		Internal::EnsureFilePathInitialized();
		return Internal::file_path;
	}

	/// Logs a Trace-level message. printf-style format string.
	template <typename... Args>
	inline void Trace(const char* format, Args... args) { Internal::FormatAndDispatch(Level::Trace, format, args...); }

	template <typename... Args>
	inline void Debug(const char* format, Args... args) { Internal::FormatAndDispatch(Level::Debug, format, args...); }

	template <typename... Args>
	inline void Info(const char* format, Args... args) { Internal::FormatAndDispatch(Level::Info, format, args...); }

	template <typename... Args>
	inline void Warn(const char* format, Args... args) { Internal::FormatAndDispatch(Level::Warn, format, args...); }

	template <typename... Args>
	inline void Error(const char* format, Args... args) { Internal::FormatAndDispatch(Level::Error, format, args...); }
}

// Contract enforcement macros. Use at the top of utility functions to warn when callers invoke
// the function before the module's Enable(). The Internal::enabled flag must be defined in the
// surrounding Internal namespace by each module.
//
// Non-void return form: provide a fallback return value.
//   MMAPI_REQUIRE_ENABLED("Animal", false);
//   MMAPI_REQUIRE_ENABLED("Spell", {});
#define MMAPI_REQUIRE_ENABLED(module_label, return_value)                          \
	do {                                                                           \
		if (!Internal::enabled)                                                    \
		{                                                                          \
			MMAPI::Log::Warn("%s called before " module_label "::Enable()",        \
				__FUNCTION__);                                                     \
			return return_value;                                                   \
		}                                                                          \
	} while (0)

// Void return form.
//   MMAPI_REQUIRE_ENABLED_VOID("Animal");
#define MMAPI_REQUIRE_ENABLED_VOID(module_label)                                   \
	do {                                                                           \
		if (!Internal::enabled)                                                    \
		{                                                                          \
			MMAPI::Log::Warn("%s called before " module_label "::Enable()",        \
				__FUNCTION__);                                                     \
			return;                                                                \
		}                                                                          \
	} while (0)
