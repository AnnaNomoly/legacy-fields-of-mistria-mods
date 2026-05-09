#pragma once

#include "Core.hpp"
#include "Engine.hpp"

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

	namespace Internal
	{
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
		if (!MMAPI::Internal::global_instance)
			return {};

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
}
