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

		using OnLocalizedStringCallback  = void(*)(MMAPI::Text::LocalizedStringContext&);
		using OnPlayConversationCallback = void(*)(MMAPI::Text::PlayConversationContext&);
		using OnPlayTextCallback         = void(*)(MMAPI::Text::PlayTextContext&);

		inline OnLocalizedStringCallback  on_localized_string_callback  = nullptr;
		inline OnPlayConversationCallback on_play_conversation_callback = nullptr;
		inline OnPlayTextCallback         on_play_text_callback         = nullptr;

		inline YYTK::RValue& GmlScriptGetLocalizerCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (Arguments && ArgumentCount >= 1 && Arguments[0])
			{
				MMAPI::Text::LocalizedStringContext context{ Arguments[0]->ToString() };
				on_localized_string_callback(context);
				*Arguments[0] = YYTK::RValue(context.m_key);
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(
					MMAPI::Internal::self_module,
					GML_SCRIPT_GET_LOCALIZER
				)
			);

			original(Self, Other, Result, ArgumentCount, Arguments);
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
				on_play_conversation_callback(context);

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

		inline Aurie::AurieStatus RegisterLocalizedStringHook(OnLocalizedStringCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_GET_LOCALIZER,
				reinterpret_cast<PVOID>(GmlScriptGetLocalizerCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			on_localized_string_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline Aurie::AurieStatus RegisterPlayConversationHook(OnPlayConversationCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_PLAY_CONVERSATION,
				reinterpret_cast<PVOID>(GmlScriptPlayConversationCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			on_play_conversation_callback = callback;
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
				on_play_text_callback(context);

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

		inline Aurie::AurieStatus RegisterPlayTextHook(OnPlayTextCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_PLAY_TEXT,
				reinterpret_cast<PVOID>(GmlScriptPlayTextCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			on_play_text_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}
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

	namespace Hooks
	{
		/// Registers a callback that can modify a localization key before the game resolves the localized string.
		/// @attention Requires MMAPI to be initialized with the AurieModule pointer via Initialize.
		/// @param callback A function called with a mutable localized string context.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus OnLocalizedString(Internal::OnLocalizedStringCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::on_localized_string_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterLocalizedStringHook(callback);
		}

		/// Registers a callback that runs before the game plays a conversation.
		/// Use ctx.SetKey() to redirect the conversation, or ctx.Cancel() to prevent it from playing entirely.
		/// @attention Requires MMAPI to be initialized with the AurieModule pointer via Initialize.
		/// @param callback A function called with a mutable conversation context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus OnPlayConversation(Internal::OnPlayConversationCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::on_play_conversation_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterPlayConversationHook(callback);
		}

		/// Registers a callback that runs before the game plays a conversation text node.
		/// Use ctx.SetKey() to redirect to a different text node, or ctx.Cancel() to prevent it from playing entirely.
		/// @attention Requires MMAPI to be initialized with the AurieModule pointer via Initialize.
		/// @param callback A function called with a mutable text context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus OnPlayText(Internal::OnPlayTextCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::on_play_text_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterPlayTextHook(callback);
		}
	}
}
