#pragma once

#include "Core.hpp"
#include "Status.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <vector>

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

		// Adjacency list of recorded dependency edges, keyed by caller name (typically a
		// Hooks::* registrar name or another module's Enable). Populated by the
		// MMAPI_ENABLE_DEPENDENCY macro and rendered by Log::DumpDependencyGraph(). Edges
		// are deduplicated so repeated cascades don't bloat the structural view.
		inline std::map<std::string, std::vector<std::string>> dependency_graph;

		inline void RecordDependencyEdge(const char* caller, const char* dependency)
		{
			if (!caller || !dependency) return;
			auto& deps = dependency_graph[caller];
			if (std::find(deps.begin(), deps.end(), dependency) == deps.end())
				deps.push_back(dependency);
		}

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
				case Level::Info:  return "INFO";
				case Level::Warn:  return "WARN";
				case Level::Error: return "ERROR";
			}
			return "?????";
		}

		constexpr const char* LevelTagPadded(Level level) noexcept
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

			file_stream << timestamp << " [" << LevelTagPadded(level) << "] " << message << '\n';
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

		/// Like FormatAndDispatch but unconditionally bypasses the console sink — writes only
		/// to the file sink, gated by current_level and the File sink being in current_sinks.
		/// Used for high-volume diagnostic output that would clutter the console (e.g. the
		/// dependency resolution edges emitted by MMAPI_ENABLE_DEPENDENCY).
		template <typename... Args>
		inline void FormatAndDispatchFileOnly(Level level, const char* format, Args... args)
		{
			if (static_cast<int>(level) < static_cast<int>(current_level))
				return;
			if (!HasSink(current_sinks, Sinks::File))
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

			WriteTimestampedLine(level, buffer);
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

	/// Logs a Debug-level message routed only to the File sink, never to Console.
	/// Gated by current_level filtering and File sink being in current_sinks; drops silently
	/// otherwise. Useful for diagnostic output that is valuable in a log file but would
	/// clutter the console — e.g. dependency-graph edges emitted by MMAPI_ENABLE_DEPENDENCY.
	template <typename... Args>
	inline void DebugFile(const char* format, Args... args) { Internal::FormatAndDispatchFileOnly(Level::Debug, format, args...); }

	/// Pretty-prints the recorded MMAPI dependency graph to the file sink only. The graph is
	/// populated by every MMAPI_ENABLE_DEPENDENCY invocation, with edges deduplicated per
	/// caller. Output groups edges by caller (typically a Hooks::* registrar name or another
	/// module's Enable) and renders each adjacency list with ASCII tree connectors.
	///
	/// Intended to be called once after the mod's initialization has settled (typically the
	/// end of ModuleInitialize). No-op if the File sink is not enabled or current_level is
	/// above Debug. Console output is never emitted regardless of sink config.
	///
	/// Sample output:
	///   === MMAPI Dependency Graph ===
	///   Recorded 11 edges across 5 callers
	///
	///   BeforeUseItem
	///   └── MMAPI::Item
	///
	///   Enable
	///   ├── MMAPI::Instance
	///   ├── MMAPI::Text
	///   └── MMAPI::Weather
	inline void DumpDependencyGraph()
	{
		if (static_cast<int>(Level::Debug) < static_cast<int>(Internal::current_level))
			return;
		if (!HasSink(Internal::current_sinks, Sinks::File))
			return;

		size_t edge_count = 0;
		for (const auto& [_, deps] : Internal::dependency_graph)
			edge_count += deps.size();

		Internal::WriteTimestampedLine(Level::Debug, "=== MMAPI Dependency Graph ===");
		char header[128];
		std::snprintf(header, sizeof(header),
			"Recorded %zu edges across %zu callers",
			edge_count, Internal::dependency_graph.size());
		Internal::WriteTimestampedLine(Level::Debug, header);
		Internal::WriteTimestampedLine(Level::Debug, "");

		for (const auto& [caller, deps] : Internal::dependency_graph)
		{
			Internal::WriteTimestampedLine(Level::Debug, caller.c_str());
			for (size_t i = 0; i < deps.size(); ++i)
			{
				const bool is_last = (i + 1 == deps.size());
				char line[Internal::kMessageBufferSize];
				std::snprintf(line, sizeof(line),
					"%s %s",
					is_last ? "\xE2\x94\x94\xE2\x94\x80\xE2\x94\x80" : "\xE2\x94\x9C\xE2\x94\x80\xE2\x94\x80",
					deps[i].c_str());
				Internal::WriteTimestampedLine(Level::Debug, line);
			}
			Internal::WriteTimestampedLine(Level::Debug, "");
		}
	}
}

// Contract enforcement macros (MMAPI_REQUIRE_ENABLED / _VOID) and the dependency cascade macro
// (MMAPI_ENABLE_DEPENDENCY) live in Core.hpp. They are infrastructure for the module Enable()
// contract — not logging primitives. Log.hpp is included transitively by all callers, so the
// macros' references to MMAPI::Log::Warn and MMAPI::Log::DebugFile resolve at expansion.
