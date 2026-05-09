#pragma once

#include "Core.hpp"

#include <string>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Text
{
	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_GET_LOCALIZER     = "gml_Script_get@Localizer@Localizer";
		inline constexpr const char* GML_SCRIPT_PLAY_CONVERSATION = "gml_Script_play_conversation";
		inline constexpr const char* GML_SCRIPT_CLOSE_TEXTBOX     = "gml_Script_begin_close@TextboxMenu@TextboxMenu";
	}

	/// Gets a localized string by localization key.
	/// @attention Requires MMAPI::Text::Internal::GML_SCRIPT_GET_LOCALIZER to be registered via RegisterScriptContext.
	/// @param localization_key The localization key to resolve.
	/// @return The localized string as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetLocalizedString(const std::string& localization_key)
	{
		const auto& refs = MMAPI::Internal::script_reference_map;
		if (!refs.contains(Internal::GML_SCRIPT_GET_LOCALIZER))
			return {};
		YYTK::CInstance* Self  = refs.at(Internal::GML_SCRIPT_GET_LOCALIZER)[0];
		YYTK::CInstance* Other = refs.at(Internal::GML_SCRIPT_GET_LOCALIZER)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GET_LOCALIZER, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		YYTK::RValue key = localization_key.c_str();
		YYTK::RValue* args[1] = { &key };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
		return result;
	}

	/// Plays a conversation by localization key.
	/// @param Self The GML instance invoking the conversation.
	/// @param Other The GML other instance context.
	/// @param conversation_key The conversation localization key to play.
	inline void PlayConversation(YYTK::CInstance* Self, YYTK::CInstance* Other, const std::string& conversation_key)
	{
		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_PLAY_CONVERSATION, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue mode = 2;
		YYTK::RValue conversation = conversation_key.c_str();
		YYTK::RValue undefined;
		YYTK::RValue result;
		YYTK::RValue* args[4] = { &mode, &conversation, &undefined, &undefined };
		gml_script->m_Functions->m_ScriptFunction(Self, Self, result, 4, args);
	}

	/// Begins closing the current textbox.
	/// @param Self The GML instance invoking the close.
	/// @param Other The GML other instance context.
	inline void CloseTextbox(YYTK::CInstance* Self, YYTK::CInstance* Other)
	{
		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_CLOSE_TEXTBOX, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
	}
}
