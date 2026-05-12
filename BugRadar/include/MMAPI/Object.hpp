#pragma once

#include "Core.hpp"
#include "Engine.hpp"
#include "Hook.hpp"
#include "Log.hpp"
#include "Status.hpp"

#include <map>
#include <optional>
#include <string>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Object
{
	struct Position
	{
		int x = 0;
		int y = 0;
	};

	struct FurniturePlacedContext
	{
		YYTK::RValue m_furniture;

		/// Returns the placed-furniture struct (the script's Result). Has members like object_id,
		/// top_left_x/y, write_size_x/y.
		YYTK::RValue GetFurniture() const { return m_furniture; }

		/// Convenience: object_id from the furniture struct, or -1 if it can't be resolved.
		int GetObjectId() const;

		/// Convenience: top-left tile position of the furniture, or std::nullopt if unavailable.
		std::optional<MMAPI::Object::Position> GetTopLeftPosition() const;
	};

	struct ObjectErasedContext
	{
		YYTK::RValue m_object;

		/// Returns the object renderer being erased (the script's Arguments[0]).
		YYTK::RValue GetObject() const { return m_object; }

		/// Convenience: object_id of the object being erased, or -1 if it can't be resolved.
		int GetObjectId() const;

		/// Convenience: top-left tile position of the object, or std::nullopt if unavailable.
		std::optional<MMAPI::Object::Position> GetTopLeftPosition() const;
	};

	namespace Internal
	{
		inline bool enabled = false;

		inline constexpr const char* GML_SCRIPT_WRITE_FURNITURE_TO_LOCATION = "gml_Script_write_furniture_to_location";
		inline constexpr const char* GML_SCRIPT_ERASE_OBJECT_RENDERER       = "gml_Script_erase_object_renderer";

		using AfterFurniturePlacedCallback = void(*)(MMAPI::Object::FurniturePlacedContext&);
		using AfterObjectErasedCallback    = void(*)(MMAPI::Object::ObjectErasedContext&);

		inline AfterFurniturePlacedCallback after_furniture_placed_callback = nullptr;
		inline AfterObjectErasedCallback    after_object_erased_callback    = nullptr;

		inline std::map<int, std::string> object_id_to_internal_name_map;
		inline std::map<std::string, int> internal_name_to_object_id_map;

		inline YYTK::RValue ResolveInternalName(int object_id)
		{
			YYTK::CScript* gml_script = nullptr;
			MMAPI::Internal::module_interface->GetNamedRoutinePointer("gml_Script_try_object_id_to_string", reinterpret_cast<PVOID*>(&gml_script));

			YYTK::RValue result;
			YYTK::RValue id = object_id;
			YYTK::RValue* args[1] = { &id };
			gml_script->m_Functions->m_ScriptFunction(nullptr, nullptr, result, 1, args);
			return result;
		}

		// Custom mod objects are absent from globalInstance.__object_id__, so we resolve via
		// gml_Script_try_object_id_to_string instead. Custom mod items do appear in __item_data.
		inline bool EnsureObjectIdCache()
		{
			if (!object_id_to_internal_name_map.empty())
				return true;

			for (int i = 0; i < 10000; i++)
			{
				YYTK::RValue internal_name = ResolveInternalName(i);
				if (internal_name.m_Kind != YYTK::VALUE_STRING)
					continue;

				std::string name = internal_name.ToString();
				object_id_to_internal_name_map[i] = name;
				internal_name_to_object_id_map[name] = i;
			}

			return !object_id_to_internal_name_map.empty();
		}
	}

	/// Gets an object internal name from an object ID.
	/// @param object_id The object ID to resolve.
	/// @return The object internal name as an RValue, or undefined if the script context is unavailable or the ID cannot be resolved.
	inline YYTK::RValue GetInternalName(int object_id)
	{
		if (Internal::object_id_to_internal_name_map.contains(object_id))
			return Internal::object_id_to_internal_name_map.at(object_id).c_str();

		YYTK::RValue internal_name = Internal::ResolveInternalName(object_id);
		if (internal_name.m_Kind == YYTK::VALUE_STRING)
		{
			std::string name = internal_name.ToString();
			Internal::object_id_to_internal_name_map[object_id] = name;
			Internal::internal_name_to_object_id_map[name] = object_id;
		}

		return internal_name;
	}

	/// Gets an object ID from an internal object name.
	/// Uses gml_Script_try_object_id_to_string so custom objects can be discovered.
	/// @param internal_name The internal object name to resolve.
	/// @return The object ID as an RValue, or undefined if the object cannot be found.
	inline YYTK::RValue GetIdFromInternalName(const std::string& internal_name)
	{
		if (!Internal::EnsureObjectIdCache())
			return {};

		if (!Internal::internal_name_to_object_id_map.contains(internal_name))
			return {};

		return Internal::internal_name_to_object_id_map.at(internal_name);
	}

	/// Returns true if an object ID resolves to the exact internal name.
	/// @param object_id The object ID to check.
	/// @param internal_name The exact internal object name to compare.
	inline bool IsInternalName(int object_id, const std::string& internal_name)
	{
		YYTK::RValue resolved_name = GetInternalName(object_id);
		if (resolved_name.m_Kind != YYTK::VALUE_STRING)
			return false;

		return resolved_name.ToString() == internal_name;
	}

	/// Returns true if an object ID resolves to an internal name containing the given text.
	/// @param object_id The object ID to check.
	/// @param name_part The internal object name fragment to search for.
	inline bool InternalNameContains(int object_id, const std::string& name_part)
	{
		YYTK::RValue resolved_name = GetInternalName(object_id);
		if (resolved_name.m_Kind != YYTK::VALUE_STRING)
			return false;

		return resolved_name.ToString().find(name_part) != std::string::npos;
	}

	/// Gets an object category ID from an internal category name.
	/// @param internal_name The internal object category name to look up.
	/// @return The object category ID as an RValue, or undefined if the category is not found.
	inline YYTK::RValue GetCategoryIdFromInternalName(const std::string& internal_name)
	{
		YYTK::RValue object_categories = MMAPI::Internal::global_instance->GetMember("__object_category__");
		size_t category_count = 0;
		MMAPI::Internal::module_interface->GetArraySize(object_categories, category_count);

		for (size_t i = 0; i < category_count; i++)
		{
			YYTK::RValue* category = nullptr;
			MMAPI::Internal::module_interface->GetArrayEntry(object_categories, i, category);

			if (category && category->ToString() == internal_name.c_str())
				return static_cast<int64_t>(i);
		}

		return {};
	}

	/// Gets the node struct from a live object renderer instance.
	/// @param instance The instance to inspect.
	/// @return The node struct as an RValue, or undefined if the instance does not have a node.
	inline YYTK::RValue GetNode(YYTK::CInstance* instance)
	{
		if (!instance)
			return {};

		YYTK::RValue value = instance->ToRValue();
		if (!MMAPI::Engine::StructVariableExists(value, "node"))
			return {};

		return value.GetMember("node");
	}

	/// Gets a node's prototype struct.
	/// @param node The node struct to inspect.
	/// @return The prototype struct as an RValue, or undefined if the node does not have a prototype.
	inline YYTK::RValue GetPrototype(YYTK::RValue node)
	{
		if (!MMAPI::Engine::StructVariableExists(node, "prototype"))
			return {};

		return node.GetMember("prototype");
	}

	/// Gets an object ID from an object struct, node struct, or live object renderer instance value.
	/// @param object_or_node The value to inspect.
	/// @return The object ID as an RValue, or undefined if no object ID can be found.
	inline YYTK::RValue GetObjectId(YYTK::RValue object_or_node)
	{
		if (MMAPI::Engine::StructVariableExists(object_or_node, "object_id"))
			return object_or_node.GetMember("object_id");

		if (MMAPI::Engine::StructVariableExists(object_or_node, "node"))
			return GetObjectId(object_or_node.GetMember("node"));

		YYTK::RValue prototype = GetPrototype(object_or_node);
		if (prototype.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		if (!MMAPI::Engine::StructVariableExists(prototype, "object_id"))
			return {};

		return prototype.GetMember("object_id");
	}

	/// Gets an object ID from a live object renderer instance.
	/// @param instance The instance to inspect.
	/// @return The object ID as an RValue, or undefined if no object ID can be found.
	inline YYTK::RValue GetObjectId(YYTK::CInstance* instance)
	{
		if (!instance)
			return {};

		return GetObjectId(instance->ToRValue());
	}

	/// Gets a category ID from an object prototype, node struct, or live object renderer instance value.
	/// @param object_or_node The value to inspect.
	/// @return The category ID as an RValue, or undefined if no category ID can be found.
	inline YYTK::RValue GetCategoryId(YYTK::RValue object_or_node)
	{
		if (MMAPI::Engine::StructVariableExists(object_or_node, "category_id"))
			return object_or_node.GetMember("category_id");

		if (MMAPI::Engine::StructVariableExists(object_or_node, "node"))
			return GetCategoryId(object_or_node.GetMember("node"));

		YYTK::RValue prototype = GetPrototype(object_or_node);
		if (prototype.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		if (!MMAPI::Engine::StructVariableExists(prototype, "category_id"))
			return {};

		return prototype.GetMember("category_id");
	}

	/// Gets a category ID from a live object renderer instance.
	/// @param instance The instance to inspect.
	/// @return The category ID as an RValue, or undefined if no category ID can be found.
	inline YYTK::RValue GetCategoryId(YYTK::CInstance* instance)
	{
		if (!instance)
			return {};

		return GetCategoryId(instance->ToRValue());
	}

	/// Gets a node or object renderer's top-left tile position.
	/// @param object_or_node The value to inspect.
	/// @return The top-left tile position, or std::nullopt if the value does not expose top_left_x/top_left_y.
	inline std::optional<MMAPI::Object::Position> GetTopLeftPosition(YYTK::RValue object_or_node)
	{
		if (MMAPI::Engine::StructVariableExists(object_or_node, "node"))
			return GetTopLeftPosition(object_or_node.GetMember("node"));

		if (!MMAPI::Engine::StructVariableExists(object_or_node, "top_left_x") ||
		    !MMAPI::Engine::StructVariableExists(object_or_node, "top_left_y"))
		{
			return std::nullopt;
		}

		return MMAPI::Object::Position{
			static_cast<int>(object_or_node.GetMember("top_left_x").ToInt64()),
			static_cast<int>(object_or_node.GetMember("top_left_y").ToInt64())
		};
	}

	/// Gets a live object renderer instance's top-left tile position.
	/// @param instance The instance to inspect.
	/// @return The top-left tile position, or std::nullopt if the instance does not expose top_left_x/top_left_y.
	inline std::optional<MMAPI::Object::Position> GetTopLeftPosition(YYTK::CInstance* instance)
	{
		if (!instance)
			return std::nullopt;

		return GetTopLeftPosition(instance->ToRValue());
	}

	/// Returns true if the node, object renderer value, or instance has node hitpoints.
	/// @param object_or_node The value to inspect.
	inline bool HasNodeHitpoints(YYTK::RValue object_or_node)
	{
		if (MMAPI::Engine::StructVariableExists(object_or_node, "node"))
			return HasNodeHitpoints(object_or_node.GetMember("node"));

		return MMAPI::Engine::StructVariableExists(object_or_node, "hitpoints");
	}

	/// Returns true if the live object renderer instance has node hitpoints.
	/// @param instance The instance to inspect.
	inline bool HasNodeHitpoints(YYTK::CInstance* instance)
	{
		if (!instance)
			return false;

		return HasNodeHitpoints(instance->ToRValue());
	}

	/// Gets node hitpoints from a node, object renderer value, or instance.
	/// @param object_or_node The value to inspect.
	/// @return The node hitpoints as an RValue, or undefined if hitpoints are unavailable.
	inline YYTK::RValue GetNodeHitpoints(YYTK::RValue object_or_node)
	{
		if (MMAPI::Engine::StructVariableExists(object_or_node, "node"))
			return GetNodeHitpoints(object_or_node.GetMember("node"));

		if (!MMAPI::Engine::StructVariableExists(object_or_node, "hitpoints"))
			return {};

		return object_or_node.GetMember("hitpoints");
	}

	/// Gets node hitpoints from a live object renderer instance.
	/// @param instance The instance to inspect.
	/// @return The node hitpoints as an RValue, or undefined if hitpoints are unavailable.
	inline YYTK::RValue GetNodeHitpoints(YYTK::CInstance* instance)
	{
		if (!instance)
			return {};

		return GetNodeHitpoints(instance->ToRValue());
	}

	/// Sets node hitpoints on a node or object renderer value.
	/// @param object_or_node The value to modify.
	/// @param hitpoints The new hitpoints value.
	/// @return True if hitpoints were found and updated.
	inline bool SetNodeHitpoints(YYTK::RValue object_or_node, int hitpoints)
	{
		if (MMAPI::Engine::StructVariableExists(object_or_node, "node"))
			return SetNodeHitpoints(object_or_node.GetMember("node"), hitpoints);

		if (!MMAPI::Engine::StructVariableExists(object_or_node, "hitpoints"))
			return false;

		*object_or_node.GetRefMember("hitpoints") = hitpoints;
		return true;
	}

	/// Sets node hitpoints on a live object renderer instance.
	/// @param instance The instance to modify.
	/// @param hitpoints The new hitpoints value.
	/// @return True if hitpoints were found and updated.
	inline bool SetNodeHitpoints(YYTK::CInstance* instance, int hitpoints)
	{
		if (!instance)
			return false;

		return SetNodeHitpoints(instance->ToRValue(), hitpoints);
	}

	/// Returns true if the node, object renderer value, or instance has a prototype marked as a ladder candidate.
	/// @param object_or_node The value to inspect.
	inline bool IsLadderCandidate(YYTK::RValue object_or_node)
	{
		if (MMAPI::Engine::StructVariableExists(object_or_node, "node"))
			return IsLadderCandidate(object_or_node.GetMember("node"));

		YYTK::RValue prototype = GetPrototype(object_or_node);
		if (prototype.m_Kind == YYTK::VALUE_UNDEFINED)
			return false;

		if (!MMAPI::Engine::StructVariableExists(prototype, "ladder_candidate"))
			return false;

		return prototype.GetMember("ladder_candidate").ToBoolean();
	}

	/// Returns true if the live object renderer instance has a prototype marked as a ladder candidate.
	/// @param instance The instance to inspect.
	inline bool IsLadderCandidate(YYTK::CInstance* instance)
	{
		if (!instance)
			return false;

		return IsLadderCandidate(instance->ToRValue());
	}

	inline int FurniturePlacedContext::GetObjectId() const
	{
		YYTK::RValue id = MMAPI::Object::GetObjectId(m_furniture);
		return MMAPI::Engine::IsNumeric(id) ? static_cast<int>(id.ToInt64()) : -1;
	}

	inline std::optional<MMAPI::Object::Position> FurniturePlacedContext::GetTopLeftPosition() const
	{
		return MMAPI::Object::GetTopLeftPosition(m_furniture);
	}

	inline int ObjectErasedContext::GetObjectId() const
	{
		YYTK::RValue id = MMAPI::Object::GetObjectId(m_object);
		return MMAPI::Engine::IsNumeric(id) ? static_cast<int>(id.ToInt64()) : -1;
	}

	inline std::optional<MMAPI::Object::Position> ObjectErasedContext::GetTopLeftPosition() const
	{
		return MMAPI::Object::GetTopLeftPosition(m_object);
	}

	namespace Internal
	{
		inline YYTK::RValue& GmlScriptAfterWriteFurnitureCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_WRITE_FURNITURE_TO_LOCATION)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			if (after_furniture_placed_callback
				&& Result.m_Kind != YYTK::VALUE_UNDEFINED
				&& Result.m_Kind != YYTK::VALUE_UNSET)
			{
				MMAPI::Object::FurniturePlacedContext context{ Result };
				after_furniture_placed_callback(context);
			}

			return Result;
		}

		inline YYTK::RValue& GmlScriptAfterEraseObjectRendererCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_ERASE_OBJECT_RENDERER)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			if (after_object_erased_callback && Arguments && ArgumentCount >= 1 && Arguments[0])
			{
				MMAPI::Object::ObjectErasedContext context{ *Arguments[0] };
				after_object_erased_callback(context);
			}

			return Result;
		}
	}

	/// Activates Object utility hooks. Eagerly installs the write_furniture_to_location and
	/// erase_object_renderer script hooks used by Hooks::AfterFurniturePlaced and
	/// Hooks::AfterObjectErased. Object's pure utility functions (GetObjectId,
	/// GetTopLeftPosition, etc.) do NOT require Enable() — they work standalone.
	/// @return Status::Success if the hooks are installed (or already were); otherwise a failure status.
	inline MMAPI::Status Enable()
	{
		if (Internal::enabled)
			return MMAPI::Status::Success;

		MMAPI::Log::Debug("MMAPI::Object::Enable() called");

		MMAPI::Status status = MMAPI::Internal::InstallScriptHooks({
			{ Internal::GML_SCRIPT_WRITE_FURNITURE_TO_LOCATION, reinterpret_cast<PVOID>(Internal::GmlScriptAfterWriteFurnitureCallback) },
			{ Internal::GML_SCRIPT_ERASE_OBJECT_RENDERER,       reinterpret_cast<PVOID>(Internal::GmlScriptAfterEraseObjectRendererCallback) },
		});
		if (!MMAPI::IsSuccess(status))
			return status;

		Internal::enabled = true;
		return MMAPI::Status::Success;
	}

	namespace Hooks
	{
		/// Registers a callback that runs after the game writes a placed piece of furniture into
		/// its location data (i.e., the player just placed furniture and it has been committed to
		/// the location's persistent state). Use `ctx.GetObjectId()` to filter by object type and
		/// `ctx.GetTopLeftPosition()` to read where it was placed.
		/// @param callback A function called with a `MMAPI::Object::FurniturePlacedContext`.
		/// @return Status::Success if the hook was installed; Status::AlreadyRegistered if a callback is already registered; otherwise a failure status.
		inline MMAPI::Status AfterFurniturePlaced(Internal::AfterFurniturePlacedCallback callback)
		{
			MMAPI::Status status = MMAPI::Object::Enable();
			if (!MMAPI::IsSuccess(status))
				return status;

			return MMAPI::Internal::RegisterHook(
				"Object::AfterFurniturePlaced",
				Internal::after_furniture_placed_callback,
				callback
			);
		}

		/// Registers a callback that runs after the game erases an object renderer (e.g., the
		/// player picks up placed furniture and the game removes its on-screen renderer). Use
		/// `ctx.GetObjectId()` to filter by object type and `ctx.GetTopLeftPosition()` to read
		/// where it was. Pairs with AfterFurniturePlaced for furniture-lifecycle tracking.
		/// @param callback A function called with a `MMAPI::Object::ObjectErasedContext`.
		/// @return Status::Success if the hook was installed; Status::AlreadyRegistered if a callback is already registered; otherwise a failure status.
		inline MMAPI::Status AfterObjectErased(Internal::AfterObjectErasedCallback callback)
		{
			MMAPI::Status status = MMAPI::Object::Enable();
			if (!MMAPI::IsSuccess(status))
				return status;

			return MMAPI::Internal::RegisterHook(
				"Object::AfterObjectErased",
				Internal::after_object_erased_callback,
				callback
			);
		}
	}
}
