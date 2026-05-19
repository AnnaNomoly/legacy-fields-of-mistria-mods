#include <map>
#include <iostream>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <excpt.h>
#include <YYToolkit/YYTK_Shared.hpp> // YYTK v4
using namespace Aurie;
using namespace YYTK;

static const char* const MOD_NAME = "DataMining";
static const char* const VERSION = "1.0.1";
static const char* GML_SCRIPT_GET_LOCALIZER = "gml_Script_get@Localizer@Localizer";
static const char* GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";
static const char* GML_SCRIPT_GET_WEATHER = "gml_Script_get_weather@WeatherManager@Weather";

static YYTKInterface* g_ModuleInterface = nullptr;
static bool run_once = true;
static bool localize_items = false;
// get_weather runs continuously in-world but never on the title screen, so its first fire after
// setup_main_screen is a reliable "world active" signal. Used to dump globals that aren't
// populated until a save loads (__pet, __grids, __npc_database).
static bool world_active_dump_pending = false;
std::string data_mining_folder = "";
static std::vector<std::string> cosmetic_names;
static std::map<std::string, int> item_name_to_id_map = {};
static std::map<std::string, std::string> item_name_to_localized_name_map = {};
static std::map<std::string, std::string> cooking_recipe_name_to_localized_name_map = {}; // All cooked dishes
static std::map<std::string, std::string> craftable_cooking_recipe_name_to_localized_name_map = {}; // Cooked dishes with recipes
static std::map<std::string, std::string> non_craftable_cooking_recipe_name_to_localized_name_map = {}; // Cooked dishes without recipes
static std::map<std::string, std::string> furniture_recipe_name_to_localized_name_map = {}; // All furniture
static std::map<std::string, std::string> craftable_furniture_recipe_name_to_localized_name_map = {}; // Craftable furniture (has a recipe)
static std::map<std::string, std::string> non_craftable_furniture_recipe_name_to_localized_name_map = {}; // Non-craftable furniture (has no recipe)
static std::map<std::string, std::string> cosmetic_name_to_localized_name_map = {};

bool GetAllCosmeticNames(
	IN const char* MemberName,
	IN OUT RValue* Value
)
{
	auto it = std::find(cosmetic_names.begin(), cosmetic_names.end(), MemberName);
	if (it == cosmetic_names.end())
	{
		cosmetic_names.push_back(MemberName);
	}
	return false;
}

int RValueAsInt(RValue value)
{
	if (value.m_Kind == VALUE_REAL)
		return static_cast<int>(value.m_Real);
	if (value.m_Kind == VALUE_INT64)
		return static_cast<int>(value.m_i64);
	if (value.m_Kind == VALUE_INT32)
		return static_cast<int>(value.m_i32);
}

bool RValueAsBool(RValue value)
{
	if (value.m_Kind == VALUE_REAL && value.m_Real == 1)
		return true;
	if (value.m_Kind == VALUE_BOOL && value.m_Real == 1)
		return true;
	return false;
}

bool StructVariableExists(RValue the_struct, const char* variable_name)
{
	RValue struct_exists = g_ModuleInterface->CallBuiltin(
		"struct_exists",
		{ the_struct, variable_name }
	);

	return RValueAsBool(struct_exists);
}

// Reads `global.__game_version` and formats it as "major.minor.patch" (e.g. "0.14.1"). Appends
// "-{pre}" if the optional `pre` field is a populated string. Returns "(unknown)" if the global
// isn't present (e.g. called before the world is loaded for some installs).
static std::string GetGameVersionString()
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);
	if (!global_instance) return "(unknown)";

	RValue gv = global_instance->GetMember("__game_version");
	if (gv.m_Kind != VALUE_OBJECT) return "(unknown)";

	std::string s =
		std::to_string(gv.GetMember("major").ToInt64()) + "." +
		std::to_string(gv.GetMember("minor").ToInt64()) + "." +
		std::to_string(gv.GetMember("patch").ToInt64());

	RValue pre = gv.GetMember("pre");
	if (pre.m_Kind == VALUE_STRING)
	{
		std::string pre_str = pre.ToString();
		if (!pre_str.empty()) s += "-" + pre_str;
	}
	return s;
}

// Writes the standard per-file header block: `# {description} (Fields of Mistria v{ver})`,
// followed by a blank line. Use right after opening an ofstream so every DataMining .md file
// gets a consistent title and game-version stamp.
static void WriteDumpHeader(std::ofstream& outfile, const std::string& description)
{
	outfile << "# " + description + " (Fields of Mistria v" + GetGameVersionString() + ")\n\n";
}

// ---- Recursive RValue tree dumper (ported from the YYTK Dumper.cpp reference design) ----
//
// Emits the same indented `- [type] name = value` tree the YYTK console dumper produces, but
// writes to an ofstream so DataMining can persist per-global dumps to disk. Format:
//   - 2-space indent per nesting level
//   - Each line: `<prefix> - [<kind>] <name>[ = <value>]`
//   - Strings quoted; numerics rendered as "%.2f" to match the reference design exactly
//   - Object pointers printed as %p, followed by a recursive dump of their members
//   - Script refs resolved to their script name when the runner can identify them
// Depth/size limits guard against pathological cycles and oversized aggregates.

#define DUMP_MAX_DEPTH         8
#define DUMP_MAX_ARRAY_LENGTH  512
#define DUMP_MAX_STRUCT_LENGTH 512

// `seen` is an optional map of object pointer -> shallowest depth visited. When non-null, the
// dumper records every VALUE_OBJECT pointer it descends into along with the depth of that
// visit. On subsequent encounters, the dumper emits an "(already dumped above)" marker and
// skips recursion only when the previous visit was at the same or shallower depth (meaning
// the previous visit had at least as much recursion budget as this one does). If we encounter
// the same pointer at a strictly shallower depth, we re-visit because we now have more
// budget — important for structs that alias each other at different recursion levels (e.g.
// `wardrobe.outfit_current` aliases `season_data.summer`). Pass nullptr to disable dedup
// entirely and produce the full Dumper.cpp-style depth-bounded tree (cycles included).
static void DumpInstanceRecursive(std::ofstream& out, const RValue& Instance, int32_t Depth, std::unordered_map<void*, int32_t>* seen);
static void DumpArrayRecursive(std::ofstream& out, const std::string& Name, const RValue& Value, int32_t Depth, std::unordered_map<void*, int32_t>* seen);

// Pivot: the layered SEH/try-catch/sidecar defenses around the full global dump are kept in
// the source for reference but disabled. They consistently failed to prevent a hard crash
// inside __npc_database[33]'s post-walk cleanup path. WriteFullGlobalDump below is now a
// minimal reference-Dumper.cpp-style walk: depth-limited, no `seen` map (cycles handled by
// DUMP_MAX_DEPTH alone), no SEH, no try/catch, no sidecar. If it still crashes, we'll know
// the walk itself is the issue and not anything the defenses were doing.
#if 0
static bool TryDumpRValueSafe(std::ofstream& out, int32_t depth, const std::string& name, const RValue& value, std::unordered_map<void*, int32_t>* seen);
static void WriteSidecarPath(const std::string& current_path);

// File-scope state for the diagnostic sidecar file used by WriteFullGlobalDump. When non-empty,
// WriteSidecarPath truncate-writes the current dump path to this file at shallow recursion
// boundaries so a hard crash that escapes SEH still leaves a breadcrumb on disk. WriteGlobalDump
// (the per-member dumps) doesn't set this, so the sidecar is a no-op for those paths.
static std::string g_dump_sidecar_path;
#endif

// Walks the runner's script table to find the CScript whose YYC function pointer matches the one
// owned by a CScriptRef. Returns nullptr if no match. O(N) over script count; only called for
// SCRIPTREF cells in the dump output.
static CScript* TryLookupScriptByFunctionPointer(PFUNC_YYGMLScript ScriptFunction)
{
	AurieStatus status = AURIE_SUCCESS;
	int script_index = 0;
	while (AurieSuccess(status))
	{
		CScript* script = nullptr;
		status = g_ModuleInterface->GetScriptData(script_index, script);
		if (!AurieSuccess(status)) break;
		if (script && script->m_Functions && script->m_Functions->m_ScriptFunction == ScriptFunction)
			return script;
		script_index++;
	}
	return nullptr;
}

static void DumpRValue(std::ofstream& out, int32_t Depth, const std::string& Name, const RValue& Value, std::unordered_map<void*, int32_t>* seen)
{
	std::string prefix(Depth * 2, ' ');
	std::string kind = const_cast<RValue&>(Value).GetKindName();

	switch (Value.m_Kind)
	{
		case VALUE_REAL:
		case VALUE_INT64:
		case VALUE_INT32:
		case VALUE_BOOL:
		{
			char buf[64];
			std::snprintf(buf, sizeof(buf), "%.2f", Value.ToDouble());
			out << prefix << " - [" << kind << "] " << Name << " = " << buf << "\n";
			break;
		}
		case VALUE_OBJECT:
		{
			YYObjectBase* object = Value.ToObject();
			if (object && object->m_ObjectKind == OBJECT_KIND_SCRIPTREF)
			{
				CScriptRef* ref = Value.ToPointer<CScriptRef*>();
				CScript* script = ref ? TryLookupScriptByFunctionPointer(ref->m_CallYYC) : nullptr;
				if (script)
					out << prefix << " - [" << kind << "] " << Name << " = script '" << script->GetName() << "'\n";
				else
					out << prefix << " - [" << kind << "] " << Name << "\n";
			}
			else
			{
				char buf[32];
				std::snprintf(buf, sizeof(buf), "%p", Value.ToPointer());

				// Cycle/alias detection. The `seen` map records the shallowest depth at which
				// each pointer was visited. We skip recursion (and emit a back-reference
				// marker) only when the previous visit was at the same or shallower depth —
				// i.e. it had at least as much recursion budget as we have now. If the same
				// pointer is encountered later at a strictly shallower depth, we re-visit
				// because the previous dump's subtree was truncated by DUMP_MAX_DEPTH and the
				// shallower path can reach deeper members.
				void* ptr = Value.ToPointer();
				bool already_dumped_at_same_or_shallower = false;
				if (seen && ptr)
				{
					auto it = seen->find(ptr);
					if (it != seen->end() && it->second <= Depth)
						already_dumped_at_same_or_shallower = true;
				}

				if (already_dumped_at_same_or_shallower)
				{
					out << prefix << " - [" << kind << "] " << Name << " = " << buf << " (already dumped above)\n";
				}
				else
				{
					out << prefix << " - [" << kind << "] " << Name << " = " << buf << "\n";
					if (Depth < DUMP_MAX_DEPTH)
					{
						if (seen && ptr) (*seen)[ptr] = Depth;
						DumpInstanceRecursive(out, Value, Depth + 1, seen);
					}
				}
			}
			break;
		}
		case VALUE_STRING:
			out << prefix << " - [" << kind << "] " << Name << " = '" << Value.ToCString() << "'\n";
			break;
		case VALUE_ARRAY:
			out << prefix << " - [" << kind << "] " << Name << "\n";
			if (Depth < DUMP_MAX_DEPTH)
				DumpArrayRecursive(out, Name, Value, Depth + 1, seen);
			break;
		default:
			out << prefix << " - [" << kind << "] " << Name << "\n";
			break;
	}
}

static void DumpArrayRecursive(std::ofstream& out, const std::string& Name, const RValue& Value, int32_t Depth, std::unordered_map<void*, int32_t>* seen)
{
	auto vec = const_cast<RValue&>(Value).ToVector();
	if (vec.size() >= DUMP_MAX_ARRAY_LENGTH)
	{
		std::string prefix(Depth * 2, ' ');
		out << prefix << " - Skipping, array has over " << DUMP_MAX_ARRAY_LENGTH << " elements\n";
		return;
	}
	for (size_t i = 0; i < vec.size(); i++)
		DumpRValue(out, Depth, Name + "[" + std::to_string(i) + "]", vec[i], seen);
}

static void DumpInstanceRecursive(std::ofstream& out, const RValue& Instance, int32_t Depth, std::unordered_map<void*, int32_t>* seen)
{
	auto members = const_cast<RValue&>(Instance).ToMap();
	if (members.size() >= DUMP_MAX_STRUCT_LENGTH)
	{
		std::string prefix(Depth * 2, ' ');
		out << prefix << " - Skipping, struct has over " << DUMP_MAX_STRUCT_LENGTH << " elements\n";
		return;
	}
	for (const auto& [key, value] : members)
		DumpRValue(out, Depth, key, value, seen);
}

// Writes a Dumper.cpp-style recursive tree dump for a single global instance member. The dump
// is wrapped in a fenced code block so the indented tree renders verbatim in markdown viewers.
// Cycle/alias detection is always on (the `seen` map collapses mutual back-references like
// NPCs <-> activity_handlers without losing structural data). No-ops if the global isn't
// present (e.g. world-active data read before a save loads).
static void WriteGlobalDump(const std::string& filename, const char* global_key, const std::string& description)
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);
	if (!global_instance) return;

	RValue val = global_instance->GetMember(global_key);
	if (val.m_Kind == VALUE_UNDEFINED || val.m_Kind == VALUE_UNSET) return;

	std::string path = data_mining_folder + "\\" + filename;
	std::remove(path.c_str());
	std::ofstream outfile(path, std::ios::out);
	if (!outfile.is_open()) return;

	WriteDumpHeader(outfile, description);
	outfile << "```\n";
	std::unordered_map<void*, int32_t> seen;
	DumpRValue(outfile, 0, global_key, val, &seen);
	outfile << "```\n";
	outfile.close();
}

// ---- Disabled defensive infrastructure for the full global dump ----
//
// Kept in source for reference but compiled out (#if 0). These were progressively added to
// survive a hard crash in the full global walk and consistently failed to prevent it:
//   - TryDumpRValueSeh: SEH __except wrapper. Catches access violations but not C++ exceptions
//     under /EHsc and not engine fast-fail / TerminateProcess paths.
//   - TryDumpRValueSafe: C++ try/catch around the SEH wrapper, to catch bad_alloc and similar.
//   - WriteSidecarPath / g_dump_sidecar_path: a sidecar text file truncate-rewritten at shallow
//     recursion boundaries so a crash that escaped both layers still left a breadcrumb.
//   - The previous WriteFullGlobalDump (also disabled below): iterated a ToMap snapshot,
//     wrapped each top-level member in TryDumpRValueSafe, flushed and updated the sidecar
//     per member, reported progress on a 5-second cadence.
//
// Three diagnostics-grade runs with these in place crashed at three different points
// (__world_mods, __pad, __npc_database[33]), the last consistently — each crash bypassed
// every defensive layer. The reference Dumper.cpp from YYTK examples walks instances with
// nothing more than a depth cap and works on its targets; pivoting WriteFullGlobalDump to
// that minimal form to see whether the defenses themselves were destabilizing the walk.
#if 0
static int TryDumpRValueSeh(
	std::ofstream* out,
	int32_t depth,
	const std::string* name,
	const RValue* value,
	std::unordered_map<void*, int32_t>* seen
)
{
	__try
	{
		DumpRValue(*out, depth, *name, *value, seen);
		return 1;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return 0;
	}
}

// Outer layer of crash protection: C++ try/catch around the SEH wrapper. With /EHsc (this
// project's default), __except catches structured exceptions like access violations but NOT
// C++ exceptions. Memory exhaustion in ToVector/ToMap (bad_alloc) is the most plausible way
// an exception escapes SEH, and that's exactly what was suspected to have killed a 38MB dump
// mid-__world_mods with no SEH marker written. Two-layer wrapping closes that gap.
static bool TryDumpRValueSafe(
	std::ofstream& out,
	int32_t depth,
	const std::string& name,
	const RValue& value,
	std::unordered_map<void*, int32_t>* seen
)
{
	try
	{
		return TryDumpRValueSeh(&out, depth, &name, &value, seen) != 0;
	}
	catch (...)
	{
		return false;
	}
}

// Diagnostic breadcrumb: truncate-writes the path of the value currently being dumped to a
// sidecar file at top-level and first-level-element boundaries. If a crash escapes both SEH
// and the C++ catch (process termination, stack-overflow handler unable to allocate), the
// sidecar file is the last record of where the dumper was. Open-write-close per call so each
// update is fully flushed to disk before continuing. No-op when g_dump_sidecar_path is empty.
static void WriteSidecarPath(const std::string& current_path)
{
	if (g_dump_sidecar_path.empty()) return;
	std::ofstream sidecar(g_dump_sidecar_path, std::ios::out | std::ios::trunc);
	if (sidecar.is_open())
		sidecar << current_path << "\n";
}

// Dumps every top-level member of the GameMaker global instance to a single markdown file.
// This is the most expensive DataMining dump by orders of magnitude (millions of lines for a
// fully-loaded save), so progress is reported to the YYTK console roughly every 5 seconds with
// elapsed and estimated-remaining time.
//
// Iteration strategy: hold the ToMap snapshot alive for the entire walk; deferring all RValue
// destructors to one batch at function exit. We tried fetching a fresh RValue via GetMember
// per iteration (mirroring WriteGlobalDump exactly); that crashed inside __npc_database[33]
// while the standalone WriteGlobalDump on the same data — called immediately before — finished
// cleanly. The difference is that the per-iteration variant runs the RValue destructor (which
// calls FREE_RValue into the GML runtime) hundreds of times in sequence; the snapshot variant
// runs them in one batch at the end. Whatever cumulative state pressure that destructor churn
// puts on GML's allocator is what's tipping it over, so we avoid creating it.
//
// `seen` is still per-member (fresh per top-level), since the shared seen map across members
// is a known cycle-detection-vs-realloc hazard and the per-walk cost of cross-member dedup
// isn't worth it for a diagnostic dump.
//
// Crash protection has three layers, in case any individual member dump goes bad anyway:
//   1. SEH (__except) catches access violations and similar structured exceptions.
//   2. A C++ try/catch around the SEH wrapper catches bad_alloc and other std exceptions.
//   3. A sidecar text file is open-truncate-written at top-level and first-level-element
//      boundaries so even a hard process termination (engine fast-fail) leaves a breadcrumb.
// The ofstream is also flushed after every top-level member so the partial dump survives.
static void WriteFullGlobalDump(const std::string& filename, const std::string& description)
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);
	if (!global_instance) return;

	std::string path = data_mining_folder + "\\" + filename;
	std::remove(path.c_str());
	std::ofstream outfile(path, std::ios::out);
	if (!outfile.is_open()) return;

	// Sidecar lives next to the dump file and shares its base name with a ".current.txt" suffix.
	// Cleared up front so a stale breadcrumb from a previous successful run can't mislead.
	g_dump_sidecar_path = path + ".current.txt";
	std::remove(g_dump_sidecar_path.c_str());

	// One-shot snapshot of all top-level members. The RValues inside live until this function
	// returns; their destructors run in one batch at end of scope, not per-iteration. See the
	// big comment above for why that matters.
	RValue global_rval = global_instance->ToRValue();
	auto members = global_rval.ToMap();
	const size_t total = members.size();

	WriteDumpHeader(outfile, description);
	outfile << "```\n";

	const auto start_time = std::chrono::steady_clock::now();
	auto last_log = start_time;
	size_t done = 0;

	g_ModuleInterface->Print(CM_LIGHTAQUA,
		"[%s %s] - Full global dump started (%zu top-level members, expect large output)",
		MOD_NAME, VERSION, total);

	size_t failed = 0;
	for (const auto& [name, value] : members)
	{
		WriteSidecarPath(name);

		// Fresh seen map per member: prevents cycle-detection from pointing into memory that
		// was freed and reused since an earlier top-level walk recorded it.
		std::unordered_map<void*, int32_t> seen;

		if (!TryDumpRValueSafe(outfile, 0, name, value, &seen))
		{
			outfile << " - !! Crashed while dumping " << name << " !!\n";
			g_ModuleInterface->Print(CM_LIGHTRED,
				"[%s %s] - SEH/C++ exception while dumping global member '%s', skipping",
				MOD_NAME, VERSION, name.c_str());
			failed++;
		}
		done++;

		// Flush after each member so partial output survives a hard crash. The cost is small
		// (once per top-level member, not per row), and being able to see which member was
		// being dumped at the time of a crash is invaluable for diagnostics.
		outfile.flush();

		// Report progress when it's been at least ~5 seconds since the last log line. Coarser
		// than per-member because some members (e.g. __npc_database) take noticeably longer
		// than others and we don't want a wall of console spam from the cheap ones.
		auto now = std::chrono::steady_clock::now();
		auto since_last_log = std::chrono::duration_cast<std::chrono::seconds>(now - last_log).count();
		if (since_last_log >= 5 && done < total)
		{
			auto elapsed_sec = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
			int64_t est_total_sec = (done > 0) ? (elapsed_sec * static_cast<int64_t>(total) / static_cast<int64_t>(done)) : 0;
			int64_t est_remaining_sec = est_total_sec - elapsed_sec;
			int percent = static_cast<int>((done * 100) / total);
			g_ModuleInterface->Print(CM_LIGHTAQUA,
				"[%s %s] - Full global dump progress: %zu/%zu (%d%%), elapsed %llds, est. remaining %llds, just finished '%s'",
				MOD_NAME, VERSION, done, total, percent,
				static_cast<long long>(elapsed_sec), static_cast<long long>(est_remaining_sec),
				name.c_str());
			last_log = now;
		}
	}

	outfile << "```\n";
	outfile.close();

	// Reaching this point means the loop completed without process termination, so the sidecar
	// is no longer useful — remove it and clear the path so DumpArrayRecursive stops writing.
	// Any future call to WriteFullGlobalDump will set it again. (If the process crashes inside
	// the loop, neither runs, and the last sidecar contents stay on disk as the breadcrumb.)
	std::remove(g_dump_sidecar_path.c_str());
	g_dump_sidecar_path.clear();

	auto end_time = std::chrono::steady_clock::now();
	auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	if (failed > 0)
	{
		g_ModuleInterface->Print(CM_LIGHTYELLOW,
			"[%s %s] - Full global dump complete with skips: %zu/%zu members succeeded (%zu skipped due to SEH/C++) in %lld ms",
			MOD_NAME, VERSION, total - failed, total, failed, static_cast<long long>(total_ms));
	}
	else
	{
		g_ModuleInterface->Print(CM_LIGHTGREEN,
			"[%s %s] - Full global dump complete: %zu top-level members in %lld ms",
			MOD_NAME, VERSION, total, static_cast<long long>(total_ms));
	}
}
#endif // disabled defensive WriteFullGlobalDump

// Minimal reference-Dumper.cpp-style full global dump. Writes every top-level member of the
// GameMaker global instance to one markdown file, depth-capped via DUMP_MAX_DEPTH, no
// `seen` map (cycles are truncated by the depth cap), no SEH, no try/catch, no sidecar.
//
// Rationale for the bare-bones approach: see the long #if 0 block above for what's been
// tried and disabled. The reference dumper's bare style works on its targets; if it crashes
// here too, the issue is something fundamental about walking `global` that no amount of
// wrapper machinery can paper over (in which case the next move is a denylist of top-level
// member names that are known to be unsafe to walk).
static void WriteFullGlobalDump(const std::string& filename, const std::string& description)
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);
	if (!global_instance) return;

	std::string path = data_mining_folder + "\\" + filename;
	std::remove(path.c_str());
	std::ofstream outfile(path, std::ios::out);
	if (!outfile.is_open()) return;

	WriteDumpHeader(outfile, description);
	outfile << "```\n";

	RValue global_rval = global_instance->ToRValue();
	auto members = global_rval.ToMap();

	g_ModuleInterface->Print(CM_LIGHTAQUA,
		"[%s %s] - Full global dump started (%zu top-level members, expect large output)",
		MOD_NAME, VERSION, members.size());

	for (const auto& [name, value] : members)
		DumpRValue(outfile, 0, name, value, nullptr);

	outfile << "```\n";
	outfile.close();

	g_ModuleInterface->Print(CM_LIGHTGREEN,
		"[%s %s] - Full global dump complete: %zu top-level members",
		MOD_NAME, VERSION, members.size());
}

RValue GetLocalizedString(CInstance* Self, CInstance* Other, std::string localization_key)
{
	CScript* gml_script_get_localizer = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_LOCALIZER,
		(PVOID*)&gml_script_get_localizer
	);

	RValue result;
	RValue input = RValue(localization_key);
	RValue* input_ptr = &input;
	gml_script_get_localizer->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &input_ptr }
	);

	return result;
}

void ObjectCallback(
	IN FWCodeEvent& CodeEvent
)
{
	auto& [self, other, code, argc, argv] = CodeEvent.Arguments();

	if (!self)
		return;

	if (!self->m_Object)
		return;

	if (!strstr(self->m_Object->m_Name, "obj_ari"))
		return;
}

RValue& GmlScriptGetLocalizerCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (localize_items)
	{
		localize_items = false;

		// All items.
		std::string items_file_name = data_mining_folder + "\\" + "items.md";
		std::remove(items_file_name.c_str());
		std::ofstream items_outfile(items_file_name, std::ios::out);
		if (items_outfile.is_open())
		{
			WriteDumpHeader(items_outfile, "Every item known to the game, with its localized name");
			items_outfile << "| ID | Internal Name | Localized Name |\n";
			items_outfile << "| --- | --- | --- |\n";
			for (auto& pair : item_name_to_localized_name_map)
			{
				RValue localized_name = GetLocalizedString(Self, Other, pair.second);
				std::string localized_name_str = localized_name.ToString();
				pair.second = localized_name_str;

				// Item ID, Internal Item Name, Localized Item Name
				items_outfile << "| " + std::to_string(item_name_to_id_map[pair.first]) + " | " + pair.first + " | " + pair.second + " |\n";
			}
		}
		items_outfile.close();

		// All cooked dishes.
		std::string cooking_file_name = data_mining_folder + "\\" + "all_cooked_dishes.md";
		std::remove(cooking_file_name.c_str());
		std::ofstream cooking_outfile(cooking_file_name, std::ios::out);
		if (cooking_outfile.is_open())
		{
			WriteDumpHeader(cooking_outfile, "Every cooked dish known to the game, with its localized name");
			cooking_outfile << "| Internal Name | Localized Name |\n";
			cooking_outfile << "| --- | --- |\n";
			for (auto& pair : cooking_recipe_name_to_localized_name_map) {
				RValue localized_name = GetLocalizedString(Self, Other, pair.second);
				std::string localized_name_str = localized_name.ToString();
				pair.second = localized_name_str;

				// Internal Recipe Name, Localized Recipe Name
				cooking_outfile << "| " + pair.first + " | " + pair.second + " |\n";
			}
		}
		cooking_outfile.close();

		// Cooked dishes with recipes.
		std::string craftable_cooking_file_name = data_mining_folder + "\\" + "craftable_cooked_dishes.md";
		std::remove(craftable_cooking_file_name.c_str());
		std::ofstream craftable_cooking_outfile(craftable_cooking_file_name, std::ios::out);
		if (craftable_cooking_outfile.is_open())
		{
			WriteDumpHeader(craftable_cooking_outfile, "Cooked dishes that the player can craft (the dish has a recipe)");
			craftable_cooking_outfile << "| Internal Name | Localized Name |\n";
			craftable_cooking_outfile << "| --- | --- |\n";
			for (auto& pair : craftable_cooking_recipe_name_to_localized_name_map) {
				RValue localized_name = GetLocalizedString(Self, Other, pair.second);
				std::string localized_name_str = localized_name.ToString();
				pair.second = localized_name_str;

				// Internal Recipe Name, Localized Recipe Name
				craftable_cooking_outfile << "| " + pair.first + " | " + pair.second + " |\n";
			}
		}
		craftable_cooking_outfile.close();

		// Cooked dishes without recipes.
		std::string non_craftable_cooking_file_name = data_mining_folder + "\\" + "non_craftable_cooked_dishes.md";
		std::remove(non_craftable_cooking_file_name.c_str());
		std::ofstream non_craftable_cooking_outfile(non_craftable_cooking_file_name, std::ios::out);
		if (non_craftable_cooking_outfile.is_open())
		{
			WriteDumpHeader(non_craftable_cooking_outfile, "Cooked dishes that the player cannot craft (the dish has no recipe)");
			non_craftable_cooking_outfile << "| Internal Name | Localized Name |\n";
			non_craftable_cooking_outfile << "| --- | --- |\n";
			for (auto& pair : non_craftable_cooking_recipe_name_to_localized_name_map) {
				RValue localized_name = GetLocalizedString(Self, Other, pair.second);
				std::string localized_name_str = localized_name.ToString();
				pair.second = localized_name_str;

				// Internal Recipe Name, Localized Recipe Name
				non_craftable_cooking_outfile << "| " + pair.first + " | " + pair.second + " |\n";
			}
		}
		non_craftable_cooking_outfile.close();

		// All Furniture
		std::string furniture_file_name = data_mining_folder + "\\" + "all_furniture.md";
		std::remove(furniture_file_name.c_str());
		std::ofstream furniture_outfile(furniture_file_name, std::ios::out);
		if (furniture_outfile.is_open())
		{
			WriteDumpHeader(furniture_outfile, "Every furniture item known to the game, with its localized name");
			furniture_outfile << "| Internal Name | Localized Name |\n";
			furniture_outfile << "| --- | --- |\n";
			for (auto& pair : furniture_recipe_name_to_localized_name_map) {
				RValue localized_name = GetLocalizedString(Self, Other, pair.second);
				std::string localized_name_str = localized_name.ToString();
				pair.second = localized_name_str;

				// Internal Recipe Name, Localized Recipe Name
				furniture_outfile << "| " + pair.first + " | " + pair.second + " |\n";
			}
		}
		furniture_outfile.close();

		// Craftable Furniture
		std::string craftable_furniture_file_name = data_mining_folder + "\\" + "craftable_furniture.md";
		std::remove(craftable_furniture_file_name.c_str());
		std::ofstream craftable_furniture_outfile(craftable_furniture_file_name, std::ios::out);
		if (craftable_furniture_outfile.is_open())
		{
			WriteDumpHeader(craftable_furniture_outfile, "Furniture items that the player can craft (the item has a recipe)");
			craftable_furniture_outfile << "| Internal Name | Localized Name |\n";
			craftable_furniture_outfile << "| --- | --- |\n";
			for (auto& pair : craftable_furniture_recipe_name_to_localized_name_map) {
				RValue localized_name = GetLocalizedString(Self, Other, pair.second);
				std::string localized_name_str = localized_name.ToString();
				pair.second = localized_name_str;

				// Internal Recipe Name, Localized Recipe Name
				craftable_furniture_outfile << "| " + pair.first + " | " + pair.second + " |\n";
			}
		}
		craftable_furniture_outfile.close();

		// Non-Craftable Furniture
		std::string non_craftable_furniture_file_name = data_mining_folder + "\\" + "non_craftable_furniture.md";
		std::remove(non_craftable_furniture_file_name.c_str());
		std::ofstream non_craftable_furniture_outfile(non_craftable_furniture_file_name, std::ios::out);
		if (non_craftable_furniture_outfile.is_open())
		{
			WriteDumpHeader(non_craftable_furniture_outfile, "Furniture items that the player cannot craft (the item has no recipe)");
			non_craftable_furniture_outfile << "| Internal Name | Localized Name |\n";
			non_craftable_furniture_outfile << "| --- | --- |\n";
			for (auto& pair : non_craftable_furniture_recipe_name_to_localized_name_map) {
				RValue localized_name = GetLocalizedString(Self, Other, pair.second);
				std::string localized_name_str = localized_name.ToString();
				pair.second = localized_name_str;

				// Internal Recipe Name, Localized Recipe Name
				non_craftable_furniture_outfile << "| " + pair.first + " | " + pair.second + " |\n";
			}
		}
		non_craftable_furniture_outfile.close();

		// Cosmetics
		std::string cosmetics_file_name = data_mining_folder + "\\" + "cosmetics.md";
		std::remove(cosmetics_file_name.c_str());
		std::ofstream cosmetics_outfile(cosmetics_file_name, std::ios::out);
		if (cosmetics_outfile.is_open())
		{
			WriteDumpHeader(cosmetics_outfile, "Every cosmetic known to the game, with its localized name");
			cosmetics_outfile << "| Internal Name | Localized Name |\n";
			cosmetics_outfile << "| --- | --- |\n";
			for (auto& pair : cosmetic_name_to_localized_name_map) {
				RValue localized_name = GetLocalizedString(Self, Other, pair.second);
				std::string localized_name_str = localized_name.ToString();
				pair.second = localized_name_str;

				// Internal Recipe Name, Localized Recipe Name
				cosmetics_outfile << "| " + pair.first + " | " + pair.second + " |\n";
			}
		}
		cosmetics_outfile.close();

		// ---- Global GML data dumps (MMAPI-tracked instance values) ----
		// Each call writes one markdown file containing a recursive Dumper.cpp-style tree dump
		// of the named global member. The set below mirrors what MMAPI's modules introspect:
		// id-lookup arrays, per-entity arrays, and singleton structs.

		WriteGlobalDump("monsters.md",          "__monster_id__",     "Every monster id known to the game, paired with its internal name");
		WriteGlobalDump("npcs.md",              "__npc_id__",         "Every NPC id known to the game, paired with its internal name");
		WriteGlobalDump("locations.md",         "__location_id__",    "Every location id known to the game, paired with its internal name");
		WriteGlobalDump("activities.md",        "__activity__",       "Every NPC activity id known to the game, paired with its internal name");
		WriteGlobalDump("weekdays.md",          "__day__",            "The seven weekdays in id order");
		WriteGlobalDump("seasons.md",           "__season__",         "The four seasons in id order");
		WriteGlobalDump("weather.md",           "__weather__",        "Every weather id known to the game, paired with its internal name");
		WriteGlobalDump("object_categories.md", "__object_category__", "Every object category id known to the game, paired with its internal name");
		WriteGlobalDump("recipe_contexts.md",   "__recipe_context__", "Every recipe (crafting) context id known to the game, paired with its internal name");

		WriteGlobalDump("spells.md",        "__spells",    "Full data for every spell known to the game");
		WriteGlobalDump("location_data.md", "__locations", "Full data for every location known to the game");
		WriteGlobalDump("dates.md",         "__dates",     "Full data for every NPC date known to the game");

		WriteGlobalDump("xp_values.md", "__xp_values", "XP rewards for each XP-granting action, keyed by action name");

		// World-active globals (__pet, __grids, __npc_database) are also tried here but typically
		// fail at this point because the world hasn't loaded yet. They re-fire from the
		// get_weather hook below, which is a reliable "world active" signal.
		WriteGlobalDump("pet.md",          "__pet",          "Pet singleton state (populated after the player acquires a pet)");
		WriteGlobalDump("grids.md",        "__grids",        "Live grid instance state (only present for locations the player has entered this session)");
		WriteGlobalDump("npc_database.md", "__npc_database", "Per-NPC runtime database (populated once the world has loaded)");
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_LOCALIZER));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptSetupMainScreenCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SETUP_MAIN_SCREEN));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (run_once)
	{
		// Try to find the mod_data directory.
		std::string current_dir = std::filesystem::current_path().string();
		std::string mod_data_folder = current_dir + "\\mod_data";
		if (!std::filesystem::exists(mod_data_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"mod_data\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, mod_data_folder.c_str());
			std::filesystem::create_directory(mod_data_folder);
		}

		// Try to find the mod_data/DataMining directory.
		data_mining_folder = mod_data_folder + "\\DataMining";
		if (!std::filesystem::exists(data_mining_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"DataMining\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, data_mining_folder.c_str());
			std::filesystem::create_directory(data_mining_folder);
		}

		CInstance* global_instance = nullptr;
		g_ModuleInterface->GetGlobalInstance(&global_instance);

		RValue __item_data = global_instance->GetMember("__item_data");
		size_t array_length;
		g_ModuleInterface->GetArraySize(__item_data, array_length);

		// Load all items.
		for (size_t i = 0; i < array_length; i++)
		{
			RValue* array_element;
			g_ModuleInterface->GetArrayEntry(__item_data, i, array_element);

			RValue name_key = array_element->GetMember("name_key"); // The item's localization key
			if (name_key.m_Kind != VALUE_NULL && name_key.m_Kind != VALUE_UNDEFINED && name_key.m_Kind != VALUE_UNSET)
			{
				RValue item_id = array_element->GetMember("item_id");
				RValue recipe_key = array_element->GetMember("recipe_key"); // The internal item name
				item_name_to_id_map[recipe_key.ToString()] = RValueAsInt(item_id);
				item_name_to_localized_name_map[recipe_key.ToString()] = name_key.ToString();

				// Cooking recipes
				if (name_key.ToString().contains("cooked_dishes"))
				{
					cooking_recipe_name_to_localized_name_map[recipe_key.ToString()] = name_key.ToString();

					if (StructVariableExists(*array_element, "recipe"))
					{
						RValue recipe = array_element->GetMember("recipe");
						if (recipe.m_Kind != VALUE_NULL && recipe.m_Kind != VALUE_UNDEFINED && recipe.m_Kind != VALUE_UNSET)
						{
							if (StructVariableExists(recipe, "item_id"))
							{
								craftable_cooking_recipe_name_to_localized_name_map[recipe_key.ToString()] = name_key.ToString();
							}
						}
						else
						{
							non_craftable_cooking_recipe_name_to_localized_name_map[recipe_key.ToString()] = name_key.ToString();
						}
					}
					else
					{
						non_craftable_cooking_recipe_name_to_localized_name_map[recipe_key.ToString()] = name_key.ToString();
					}
				}

				// Furniture recipes.
				if (name_key.ToString().contains("furniture"))
				{
					furniture_recipe_name_to_localized_name_map[recipe_key.ToString()] = name_key.ToString();

					if (StructVariableExists(*array_element, "recipe"))
					{
						RValue recipe = array_element->GetMember("recipe");
						if (recipe.m_Kind != VALUE_NULL && recipe.m_Kind != VALUE_UNDEFINED && recipe.m_Kind != VALUE_UNSET)
						{
							if (StructVariableExists(recipe, "item_id"))
							{
								craftable_furniture_recipe_name_to_localized_name_map[recipe_key.ToString()] = name_key.ToString();
							}
						}
						else
						{
							non_craftable_furniture_recipe_name_to_localized_name_map[recipe_key.ToString()] = name_key.ToString();
						}
					}
					else
					{
						non_craftable_furniture_recipe_name_to_localized_name_map[recipe_key.ToString()] = name_key.ToString();
					}
				}
			}
		}

		// Load all cosmetics.
		RValue __pad = global_instance->GetMember("__pad");
		RValue player_assets = __pad.GetMember("player_assets");
		RValue inner = player_assets.GetMember("inner");
		g_ModuleInterface->EnumInstanceMembers(inner, GetAllCosmeticNames);
		for (std::string cosmetic_name : cosmetic_names)
		{
			RValue cosmetic = inner.GetMember(cosmetic_name);
			RValue cosmetic_localization_key = cosmetic.GetMember("name");
			cosmetic_name_to_localized_name_map[cosmetic_name] = cosmetic_localization_key.ToString();
		}

		run_once = false;
		localize_items = true;
		world_active_dump_pending = true;
	}

	return Result;
}

RValue& GmlScriptGetWeatherCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_WEATHER));
	original(Self, Other, Result, ArgumentCount, Arguments);

	if (world_active_dump_pending)
	{
		world_active_dump_pending = false;

		// World-active globals: populated only after a save loads, not on the title screen.
		WriteGlobalDump("pet.md",          "__pet",          "Pet singleton state (populated after the player acquires a pet)");
		WriteGlobalDump("grids.md",        "__grids",        "Live grid instance state (only present for locations the player has entered this session)");
		WriteGlobalDump("npc_database.md", "__npc_database", "Per-NPC runtime database (populated once the world has loaded)");

		// Comprehensive dump of every member of the global instance. Slow (multi-MB output)
		// and intentionally runs last so all the focused per-key dumps finish first. Progress
		// is reported to the YYTK console.
		WriteFullGlobalDump("global_namespace_dump.md", "Recursive dump of every member of the GameMaker global instance");
	}

	return Result;
}

void CreateHookGmlScriptGetLocalizer(AurieStatus& status)
{
	CScript* gml_script_get_localizer = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_LOCALIZER,
		(PVOID*)&gml_script_get_localizer
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_LOCALIZER);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_LOCALIZER,
		gml_script_get_localizer->m_Functions->m_ScriptFunction,
		GmlScriptGetLocalizerCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_LOCALIZER);
	}
}

void CreateHookGmlScriptSetupMainScreen(AurieStatus& status)
{
	CScript* gml_script_setup_main_screen = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SETUP_MAIN_SCREEN,
		(PVOID*)&gml_script_setup_main_screen
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SETUP_MAIN_SCREEN);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_SETUP_MAIN_SCREEN,
		gml_script_setup_main_screen->m_Functions->m_ScriptFunction,
		GmlScriptSetupMainScreenCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SETUP_MAIN_SCREEN);
	}
}

void CreateHookGmlScriptGetWeather(AurieStatus& status)
{
	CScript* gml_script_get_weather = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_WEATHER,
		(PVOID*)&gml_script_get_weather
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_WEATHER);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_WEATHER,
		gml_script_get_weather->m_Functions->m_ScriptFunction,
		GmlScriptGetWeatherCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_WEATHER);
	}
}


EXPORTED AurieStatus ModuleInitialize(
	IN AurieModule* Module,
	IN const fs::path& ModulePath
)
{
	UNREFERENCED_PARAMETER(ModulePath);

	AurieStatus status = AURIE_SUCCESS;

	status = ObGetInterface(
		"YYTK_Main",
		(AurieInterfaceBase*&)(g_ModuleInterface)
	);

	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Hello from PluginEntry!", MOD_NAME, VERSION);

	CreateHookGmlScriptGetLocalizer(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptSetupMainScreen(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetWeather(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Failed to register callback!", MOD_NAME, VERSION);
	}

	return AURIE_SUCCESS;
}