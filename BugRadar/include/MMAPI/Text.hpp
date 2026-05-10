#pragma once

#include "Core.hpp"

#include <string>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Text
{
	struct LocalizedStringContext
	{
		std::string m_key;

		std::string_view GetKey() const { return m_key; }
		void SetKey(std::string key) { m_key = std::move(key); }
	};

	struct AfterLocalizedStringContext
	{
		std::string m_key;
		std::string m_resolved;

		std::string_view GetKey() const { return m_key; }
		std::string_view GetResolved() const { return m_resolved; }
		void SetResolved(std::string resolved) { m_resolved = std::move(resolved); }
	};

	struct PlayConversationContext
	{
		std::string m_key;
		bool m_cancelled = false;

		std::string_view GetKey() const { return m_key; }
		void SetKey(std::string key) { m_key = std::move(key); }
		void Cancel() { m_cancelled = true; }
		bool IsCancelled() const { return m_cancelled; }
	};

	struct PlayTextContext
	{
		std::string m_key;
		bool m_cancelled = false;

		std::string_view GetKey() const { return m_key; }
		void SetKey(std::string key) { m_key = std::move(key); }
		void Cancel() { m_cancelled = true; }
		bool IsCancelled() const { return m_cancelled; }
	};

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_GET_LOCALIZER     = "gml_Script_get@Localizer@Localizer";
		inline constexpr const char* GML_SCRIPT_PLAY_CONVERSATION = "gml_Script_play_conversation";
		inline constexpr const char* GML_SCRIPT_CLOSE_TEXTBOX     = "gml_Script_begin_close@TextboxMenu@TextboxMenu";
		inline constexpr const char* GML_SCRIPT_PLAY_TEXT         = "gml_Script_play_text@TextboxMenu@TextboxMenu";

		using BeforeLocalizedStringCallback  = void(*)(MMAPI::Text::LocalizedStringContext&);
		using AfterLocalizedStringCallback   = void(*)(MMAPI::Text::AfterLocalizedStringContext&);
		using BeforePlayConversationCallback = void(*)(MMAPI::Text::PlayConversationContext&);
		using BeforePlayTextCallback         = void(*)(MMAPI::Text::PlayTextContext&);

		inline BeforeLocalizedStringCallback  before_localized_string_callback  = nullptr;
		inline AfterLocalizedStringCallback   after_localized_string_callback   = nullptr;
		inline BeforePlayConversationCallback before_play_conversation_callback = nullptr;
		inline BeforePlayTextCallback         before_play_text_callback         = nullptr;

		inline YYTK::RValue& GmlScriptGetLocalizerCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			MMAPI::Internal::RegisterScriptContext(GML_SCRIPT_GET_LOCALIZER, Self, Other);

			if (before_localized_string_callback && Arguments && ArgumentCount >= 1 && Arguments[0])
			{
				MMAPI::Text::LocalizedStringContext context{ Arguments[0]->ToString() };
				before_localized_string_callback(context);
				*Arguments[0] = YYTK::RValue(context.m_key);
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(
					MMAPI::Internal::self_module,
					GML_SCRIPT_GET_LOCALIZER
				)
			);

			original(Self, Other, Result, ArgumentCount, Arguments);

			if (after_localized_string_callback && Arguments && ArgumentCount >= 1 && Arguments[0])
			{
				MMAPI::Text::AfterLocalizedStringContext context{
					Arguments[0]->ToString(),
					Result.m_Kind == YYTK::VALUE_STRING ? Result.ToString() : ""
				};
				after_localized_string_callback(context);
				Result = YYTK::RValue(context.m_resolved);
			}

			return Result;
		}

		inline YYTK::RValue& GmlScriptPlayConversationCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (Arguments && ArgumentCount >= 2 && Arguments[1])
			{
				MMAPI::Text::PlayConversationContext context{ Arguments[1]->ToString() };
				before_play_conversation_callback(context);

				if (context.m_cancelled)
					return Result;

				*Arguments[1] = YYTK::RValue(context.m_key);
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(
					MMAPI::Internal::self_module,
					GML_SCRIPT_PLAY_CONVERSATION
				)
			);

			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}

		inline Aurie::AurieStatus RegisterLocalizedStringHook(BeforeLocalizedStringCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_GET_LOCALIZER,
				reinterpret_cast<PVOID>(GmlScriptGetLocalizerCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_localized_string_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline Aurie::AurieStatus RegisterPlayConversationHook(BeforePlayConversationCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_PLAY_CONVERSATION,
				reinterpret_cast<PVOID>(GmlScriptPlayConversationCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_play_conversation_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptPlayTextCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (Arguments && ArgumentCount >= 1 && Arguments[0])
			{
				MMAPI::Text::PlayTextContext context{ Arguments[0]->ToString() };
				before_play_text_callback(context);

				if (context.m_cancelled)
					return Result;

				*Arguments[0] = YYTK::RValue(context.m_key);
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_PLAY_TEXT)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			return Result;
		}

		inline Aurie::AurieStatus RegisterPlayTextHook(BeforePlayTextCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_PLAY_TEXT,
				reinterpret_cast<PVOID>(GmlScriptPlayTextCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_play_text_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}
	}

	/// Activates Text utility functions that directly call game scripts.
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		return MMAPI::Internal::InstallScriptHook(
			Internal::GML_SCRIPT_GET_LOCALIZER,
			reinterpret_cast<PVOID>(Internal::GmlScriptGetLocalizerCallback)
		);
	}

	/// Gets a localized string by localization key.
	/// @attention Requires MMAPI::Text::Enable() to have been called.
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

	namespace Hooks
	{
		/// Registers a callback that can modify a localization key before the game resolves the localized string.
		/// @param callback A function called with a mutable localized string context.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeLocalizedString(Internal::BeforeLocalizedStringCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_localized_string_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterLocalizedStringHook(callback);
		}

		/// Registers a callback that can modify a resolved localized string after the game's Localizer returns it.
		/// Use ctx.SetResolved() to substitute the string the game receives — useful for placeholder substitution in localized templates.
		/// @param callback A function called with a mutable `MMAPI::Text::AfterLocalizedStringContext`.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterLocalizedString(Internal::AfterLocalizedStringCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_localized_string_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				Internal::GML_SCRIPT_GET_LOCALIZER,
				reinterpret_cast<PVOID>(Internal::GmlScriptGetLocalizerCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			Internal::after_localized_string_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		/// Registers a callback that runs before the game plays a conversation.
		/// Use ctx.SetKey() to redirect the conversation, or ctx.Cancel() to prevent it from playing entirely.
		/// @param callback A function called with a mutable conversation context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforePlayConversation(Internal::BeforePlayConversationCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_play_conversation_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterPlayConversationHook(callback);
		}

		/// Registers a callback that runs before the game plays a conversation text node.
		/// Use ctx.SetKey() to redirect to a different text node, or ctx.Cancel() to prevent it from playing entirely.
		/// @param callback A function called with a mutable text context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforePlayText(Internal::BeforePlayTextCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_play_text_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterPlayTextHook(callback);
		}
	}
}
