#pragma once

#include "Core.hpp"
#include "Instance.hpp"

#include <string>
#include <unordered_map>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Game
{
	/// Source: globalInstance.__xp_values
	/// Struct-backed enum; values follow alphabetical key order.
	enum class XpValues : int
	{
		BreakDigSpot     = 0,
		CatchGiantFish   = 1,
		CatchLargeFish   = 2,
		CatchMediumFish  = 3,
		CatchSmallFish   = 4,
		HarvestCrop      = 5,
		HarvestDiveSpot  = 6,
		IdentifyArtifact = 7,
		PlantSeed        = 8,
		TillDirt         = 9,
		WaterDirt        = 10
	};

	struct SaveGameContext
	{
		bool m_cancelled = false;

		void Cancel() { m_cancelled = true; }
		bool IsCancelled() const { return m_cancelled; }
	};

	struct PlayAudioContext
	{
		std::string m_audio_name;
		bool m_cancelled = false;

		std::string_view GetAudioName() const { return m_audio_name; }
		void SetAudioName(std::string name) { m_audio_name = std::move(name); }
		void Cancel() { m_cancelled = true; }
		bool IsCancelled() const { return m_cancelled; }
	};

	struct HudShouldShowContext
	{
		bool m_result = true;

		bool GetResult() const { return m_result; }
		void SetResult(bool result) { m_result = result; }
	};

	namespace Internal
	{
		inline std::unordered_map<std::string, uint64_t> notification_last_display_time;
		inline constexpr const char* GML_SCRIPT_CREATE_NOTIFICATION     = "gml_Script_create_notification";
		inline constexpr const char* GML_SCRIPT_SELL_SHIPPING_BIN_ITEMS = "gml_Script_sell_shipping_bin_items";
		inline constexpr const char* GML_SCRIPT_END_DAY                 = "gml_Script_end_day";
		inline constexpr const char* GML_SCRIPT_ARI_ON_NEW_DAY          = "gml_Script_on_new_day@Ari@Ari";
		inline constexpr const char* GML_SCRIPT_LOAD_GAME               = "gml_Script_load_game";
		inline constexpr const char* GML_SCRIPT_SAVE_GAME               = "gml_Script_save_game";
		inline constexpr const char* GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY = "gml_Script_play@SceneAudioPlayer@SceneAudioPlayer";
		inline constexpr const char* GML_SCRIPT_ON_DRAW_GUI             = "gml_Script_on_draw_gui@Display@Display";
		inline constexpr const char* GML_SCRIPT_HUD_SHOULD_SHOW         = "gml_Script_hud_should_show";
		inline constexpr const char* GML_SCRIPT_CRAFTING_MENU_OPEN      = "gml_Script_initialize@CraftingMenu@CraftingMenu";
		inline constexpr const char* GML_SCRIPT_CRAFTING_MENU_CLOSE     = "gml_Script_on_close@CraftingMenu@CraftingMenu";
		inline constexpr const char* GML_SCRIPT_JOURNAL_MENU_OPEN       = "gml_Script_initialize@JournalMenu@JournalMenu";
		inline constexpr const char* GML_SCRIPT_JOURNAL_MENU_CLOSE      = "gml_Script_on_close@JournalMenu@JournalMenu";
		inline constexpr const char* GML_SCRIPT_STORE_MENU_OPEN         = "gml_Script_init@StoreMenu@StoreMenu";
		inline constexpr const char* GML_SCRIPT_STORE_MENU_CLOSE        = "gml_Script_anon@10878@StoreMenu@StoreMenu";
		inline constexpr const char* GML_SCRIPT_SETUP_MAIN_SCREEN       = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";

		using EndDayCallback = void(*)();
		using NewDayCallback = void(*)();

		inline EndDayCallback after_end_day_callback = nullptr;
		inline NewDayCallback before_new_day_callback = nullptr;

		using LoadGameCallback    = void(*)();
		using BeforeSaveGameCallback  = void(*)(MMAPI::Game::SaveGameContext&);
		using BeforePlayAudioCallback = void(*)(MMAPI::Game::PlayAudioContext&);

		inline LoadGameCallback    after_load_game_callback  = nullptr;
		inline BeforeSaveGameCallback  before_save_game_callback  = nullptr;
		inline BeforePlayAudioCallback before_play_audio_callback = nullptr;

		using AfterDrawGuiCallback          = void(*)();
		using AfterHudShouldShowCallback    = void(*)(MMAPI::Game::HudShouldShowContext&);
		using AfterCraftingMenuOpenCallback  = void(*)();
		using AfterCraftingMenuCloseCallback = void(*)();
		using AfterJournalMenuOpenCallback   = void(*)();
		using AfterJournalMenuCloseCallback  = void(*)();
		using AfterStoreMenuOpenCallback     = void(*)();
		using AfterStoreMenuCloseCallback    = void(*)();

		inline AfterDrawGuiCallback          after_draw_gui_callback           = nullptr;
		inline AfterHudShouldShowCallback    after_hud_should_show_callback    = nullptr;
		inline AfterCraftingMenuOpenCallback  after_crafting_menu_open_callback  = nullptr;
		inline AfterCraftingMenuCloseCallback after_crafting_menu_close_callback = nullptr;
		inline AfterJournalMenuOpenCallback   after_journal_menu_open_callback   = nullptr;
		inline AfterJournalMenuCloseCallback  after_journal_menu_close_callback  = nullptr;
		inline AfterStoreMenuOpenCallback     after_store_menu_open_callback     = nullptr;
		inline AfterStoreMenuCloseCallback    after_store_menu_close_callback    = nullptr;

		using BeforeSetupMainScreenCallback = void(*)();
		inline BeforeSetupMainScreenCallback before_setup_main_screen_callback = nullptr;

		inline constexpr const char* ToGameKey(MMAPI::Game::XpValues value)
		{
			switch (value)
			{
				case MMAPI::Game::XpValues::BreakDigSpot:
					return "break_dig_spot";
				case MMAPI::Game::XpValues::CatchGiantFish:
					return "catch_giant_fish";
				case MMAPI::Game::XpValues::CatchLargeFish:
					return "catch_large_fish";
				case MMAPI::Game::XpValues::CatchMediumFish:
					return "catch_medium_fish";
				case MMAPI::Game::XpValues::CatchSmallFish:
					return "catch_small_fish";
				case MMAPI::Game::XpValues::HarvestCrop:
					return "harvest_crop";
				case MMAPI::Game::XpValues::HarvestDiveSpot:
					return "harvest_dive_spot";
				case MMAPI::Game::XpValues::IdentifyArtifact:
					return "identify_artifact";
				case MMAPI::Game::XpValues::PlantSeed:
					return "plant_seed";
				case MMAPI::Game::XpValues::TillDirt:
					return "till_dirt";
				case MMAPI::Game::XpValues::WaterDirt:
					return "water_dirt";
				default:
					return nullptr;
			}
		}

		inline YYTK::RValue& GmlScriptEndDayCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(
					MMAPI::Internal::self_module,
					GML_SCRIPT_END_DAY
				)
			);

			original(Self, Other, Result, ArgumentCount, Arguments);

			after_end_day_callback();

			return Result;
		}

		inline Aurie::AurieStatus RegisterEndDayHook(EndDayCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_END_DAY,
				reinterpret_cast<PVOID>(GmlScriptEndDayCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			after_end_day_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptAriBeforeNewDayCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			before_new_day_callback();

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(
					MMAPI::Internal::self_module,
					GML_SCRIPT_ARI_ON_NEW_DAY
				)
			);

			original(Self, Other, Result, ArgumentCount, Arguments);

			return Result;
		}

		inline Aurie::AurieStatus RegisterNewDayHook(NewDayCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_ARI_ON_NEW_DAY,
				reinterpret_cast<PVOID>(GmlScriptAriBeforeNewDayCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_new_day_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptLoadGameCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_LOAD_GAME)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			after_load_game_callback();

			return Result;
		}

		inline Aurie::AurieStatus RegisterLoadGameHook(LoadGameCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_LOAD_GAME,
				reinterpret_cast<PVOID>(GmlScriptLoadGameCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			after_load_game_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptSaveGameCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			MMAPI::Game::SaveGameContext context;
			before_save_game_callback(context);

			if (context.m_cancelled)
				return Result;

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_SAVE_GAME)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			return Result;
		}

		inline Aurie::AurieStatus RegisterSaveGameHook(BeforeSaveGameCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_SAVE_GAME,
				reinterpret_cast<PVOID>(GmlScriptSaveGameCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_save_game_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptPlayAudioCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (Arguments && ArgumentCount >= 1 && Arguments[0])
			{
				MMAPI::Game::PlayAudioContext context{ Arguments[0]->ToString() };
				before_play_audio_callback(context);

				if (context.m_cancelled)
					return Result;

				*Arguments[0] = YYTK::RValue(context.m_audio_name);
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			return Result;
		}

		inline Aurie::AurieStatus RegisterPlayAudioHook(BeforePlayAudioCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY,
				reinterpret_cast<PVOID>(GmlScriptPlayAudioCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_play_audio_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptAfterDrawGuiCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_ON_DRAW_GUI)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			after_draw_gui_callback();

			return Result;
		}

		inline Aurie::AurieStatus RegisterDrawGuiHook(AfterDrawGuiCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_ON_DRAW_GUI,
				reinterpret_cast<PVOID>(GmlScriptAfterDrawGuiCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			after_draw_gui_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptHudShouldShowCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_HUD_SHOULD_SHOW)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			MMAPI::Game::HudShouldShowContext context{ Result.ToBoolean() };
			after_hud_should_show_callback(context);
			Result = context.m_result;

			return Result;
		}

		inline Aurie::AurieStatus RegisterHudShouldShowHook(AfterHudShouldShowCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_HUD_SHOULD_SHOW,
				reinterpret_cast<PVOID>(GmlScriptHudShouldShowCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			after_hud_should_show_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptCraftingMenuOpenCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_CRAFTING_MENU_OPEN)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);
			after_crafting_menu_open_callback();
			return Result;
		}

		inline Aurie::AurieStatus RegisterCraftingMenuOpenHook(AfterCraftingMenuOpenCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_CRAFTING_MENU_OPEN, reinterpret_cast<PVOID>(GmlScriptCraftingMenuOpenCallback)
			);
			if (!Aurie::AurieSuccess(status))
				return status;
			after_crafting_menu_open_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptCraftingMenuCloseCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_CRAFTING_MENU_CLOSE)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);
			after_crafting_menu_close_callback();
			return Result;
		}

		inline Aurie::AurieStatus RegisterCraftingMenuCloseHook(AfterCraftingMenuCloseCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_CRAFTING_MENU_CLOSE, reinterpret_cast<PVOID>(GmlScriptCraftingMenuCloseCallback)
			);
			if (!Aurie::AurieSuccess(status))
				return status;
			after_crafting_menu_close_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptJournalMenuOpenCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_JOURNAL_MENU_OPEN)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);
			after_journal_menu_open_callback();
			return Result;
		}

		inline Aurie::AurieStatus RegisterJournalMenuOpenHook(AfterJournalMenuOpenCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_JOURNAL_MENU_OPEN, reinterpret_cast<PVOID>(GmlScriptJournalMenuOpenCallback)
			);
			if (!Aurie::AurieSuccess(status))
				return status;
			after_journal_menu_open_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptJournalMenuCloseCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_JOURNAL_MENU_CLOSE)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);
			after_journal_menu_close_callback();
			return Result;
		}

		inline Aurie::AurieStatus RegisterJournalMenuCloseHook(AfterJournalMenuCloseCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_JOURNAL_MENU_CLOSE, reinterpret_cast<PVOID>(GmlScriptJournalMenuCloseCallback)
			);
			if (!Aurie::AurieSuccess(status))
				return status;
			after_journal_menu_close_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptStoreMenuOpenCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_STORE_MENU_OPEN)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);
			after_store_menu_open_callback();
			return Result;
		}

		inline Aurie::AurieStatus RegisterStoreMenuOpenHook(AfterStoreMenuOpenCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_STORE_MENU_OPEN, reinterpret_cast<PVOID>(GmlScriptStoreMenuOpenCallback)
			);
			if (!Aurie::AurieSuccess(status))
				return status;
			after_store_menu_open_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptStoreMenuCloseCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_STORE_MENU_CLOSE)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);
			after_store_menu_close_callback();
			return Result;
		}

		inline Aurie::AurieStatus RegisterStoreMenuCloseHook(AfterStoreMenuCloseCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_STORE_MENU_CLOSE, reinterpret_cast<PVOID>(GmlScriptStoreMenuCloseCallback)
			);
			if (!Aurie::AurieSuccess(status))
				return status;
			after_store_menu_close_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptBeforeSetupMainScreenCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			MMAPI::ClearScriptContexts();

			if (before_setup_main_screen_callback)
				before_setup_main_screen_callback();

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_SETUP_MAIN_SCREEN)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}

		inline Aurie::AurieStatus RegisterBeforeSetupMainScreenHook(BeforeSetupMainScreenCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_SETUP_MAIN_SCREEN,
				reinterpret_cast<PVOID>(GmlScriptBeforeSetupMainScreenCallback)
			);
			if (!Aurie::AurieSuccess(status))
				return status;
			before_setup_main_screen_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}
	}

	/// Returns true if the game is currently paused.
	inline bool IsPaused()
	{
		if (!MMAPI::Internal::global_instance)
			return false;

		YYTK::RValue pause_status = MMAPI::Internal::global_instance->GetMember("__pause_status");
		return pause_status.ToInt64() > 0;
	}

	/// Returns true if the game window currently has focus.
	inline bool WindowHasFocus()
	{
		YYTK::RValue window_has_focus = MMAPI::Internal::module_interface->CallBuiltin("window_has_focus", {});
		return window_has_focus.ToBoolean();
	}

	/// Returns the current game clock time in seconds from MMAPI::Internal::global_instance.__clock.time.
	inline int GetCurrentTimeInSeconds()
	{
		return static_cast<int>(
			MMAPI::Internal::global_instance
				->GetMember("__clock")
				.GetMember("time")
				.ToInt64()
		);
	}

	/// Returns the current GM room name.
	/// @return The current GM room name, or an empty string if it cannot be read.
	inline std::string GetCurrentRoomName()
	{
		if (!MMAPI::Internal::module_interface)
			return "";

		YYTK::RValue room_id;
		Aurie::AurieStatus status = MMAPI::Internal::module_interface->GetBuiltin("room", nullptr, NULL_INDEX, room_id);
		if (!Aurie::AurieSuccess(status))
			return "";

		YYTK::RValue room_name = MMAPI::Internal::module_interface->CallBuiltin("room_get_name", { room_id });
		if (room_name.m_Kind == YYTK::VALUE_UNDEFINED || room_name.m_Kind == YYTK::VALUE_UNSET)
			return "";

		return room_name.ToString();
	}

	/// Returns true if the current GM room name exactly matches room_name.
	/// @param room_name The GM room name to compare against.
	inline bool IsCurrentRoom(const std::string& room_name)
	{
		return GetCurrentRoomName() == room_name;
	}

	/// Returns true if the current GM room name contains room_name_part.
	/// @param room_name_part The text to search for in the current GM room name.
	inline bool CurrentRoomNameContains(const std::string& room_name_part)
	{
		return GetCurrentRoomName().contains(room_name_part);
	}

	/// Gets an XP value from MMAPI::Internal::global_instance.__xp_values.
	/// @param xp_value The XP value to read.
	/// @return The XP value as an RValue, or undefined if unavailable.
	inline YYTK::RValue GetXpValue(MMAPI::Game::XpValues xp_value)
	{
		if (!MMAPI::Internal::global_instance)
			return {};

		const char* xp_value_key = Internal::ToGameKey(xp_value);
		if (!xp_value_key)
			return {};

		YYTK::RValue xp_values = MMAPI::Internal::global_instance->GetMember("__xp_values");
		if (xp_values.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		return xp_values.GetMember(xp_value_key);
	}

	/// Displays a localized notification popup.
	/// @param Self The GML instance invoking the notification (passed through to the script call).
	/// @param Other The GML other instance context (passed through to the script call).
	/// @param ignore_cooldown When true, bypasses the 5-second per-key cooldown and always displays the notification.
	/// @param notification_key Localization string key for the notification text.
	inline void CreateNotification(
		YYTK::CInstance* Self,
		YYTK::CInstance* Other,
		bool ignore_cooldown,
		const std::string& notification_key
	)
	{
		uint64_t now = MMAPI::Internal::GetCurrentSystemTime();
		if (!ignore_cooldown && now <= Internal::notification_last_display_time[notification_key] + 5000)
			return;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(
			Internal::GML_SCRIPT_CREATE_NOTIFICATION,
			reinterpret_cast<PVOID*>(&gml_script)
		);

		YYTK::RValue result;
		YYTK::RValue notification_rv(notification_key);
		YYTK::RValue* notification_ptr = &notification_rv;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, { &notification_ptr });

		Internal::notification_last_display_time[notification_key] = now;
	}

	/// Activates Game utility functions that directly call game scripts.
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		return MMAPI::Instance::Enable();
	}

	/// Sells the current shipping bin contents.
	/// @attention Requires MMAPI::Game::Enable() to have been called.
	/// @return The sale result as an RValue, or undefined if obj_ari has not been registered.
	inline YYTK::RValue SellShippingBinItems()
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI) || !MMAPI::Internal::global_instance)
			return {};
		YYTK::CInstance* Self  = MMAPI::Internal::global_instance->GetRefMember("__ari")->ToInstance();
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_SELL_SHIPPING_BIN_ITEMS, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	namespace Hooks
	{
		/// Registers a callback that runs after the game ends the current day.
		/// @param callback A function called after the game's end day script runs.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterEndDay(Internal::EndDayCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_end_day_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterEndDayHook(callback);
		}

		/// Registers a callback that runs before the game starts a new day for Ari.
		/// @param callback A function called before Ari's on new day script runs.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeNewDay(Internal::NewDayCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_new_day_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterNewDayHook(callback);
		}

		/// Registers a callback that runs after the game loads a save file.
		/// @param callback A function called after the game's load_game script runs.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterLoadGame(Internal::LoadGameCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_load_game_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterLoadGameHook(callback);
		}

		/// Registers a callback that runs before the game saves.
		/// Call ctx.Cancel() to prevent the game from saving.
		/// @param callback A function called with a mutable save context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeSaveGame(Internal::BeforeSaveGameCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_save_game_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterSaveGameHook(callback);
		}

		/// Registers a callback that runs before the game plays an audio track.
		/// Use ctx.SetAudioName() to redirect to a different audio asset, or ctx.Cancel() to prevent playback.
		/// @param callback A function called with a mutable audio context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforePlayAudio(Internal::BeforePlayAudioCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_play_audio_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterPlayAudioHook(callback);
		}

		/// Registers a callback that runs after each GUI draw step.
		/// @param callback A function called after the game's draw GUI script runs.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterDrawGui(Internal::AfterDrawGuiCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_draw_gui_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterDrawGuiHook(callback);
		}

		/// Registers a callback that runs after the game evaluates whether the HUD should be shown.
		/// Use ctx.SetResult(false) to hide the HUD.
		/// @param callback A function called with a mutable HUD visibility context after the game evaluates it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterHudShouldShow(Internal::AfterHudShouldShowCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_hud_should_show_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterHudShouldShowHook(callback);
		}

		/// Registers a callback that runs after the crafting menu opens.
		/// @param callback A function called after the crafting menu's initialize script runs.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterCraftingMenuOpen(Internal::AfterCraftingMenuOpenCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_crafting_menu_open_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterCraftingMenuOpenHook(callback);
		}

		/// Registers a callback that runs after the crafting menu closes.
		/// @param callback A function called after the crafting menu's close script runs.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterCraftingMenuClose(Internal::AfterCraftingMenuCloseCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_crafting_menu_close_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterCraftingMenuCloseHook(callback);
		}

		/// Registers a callback that runs after the journal menu opens.
		/// @param callback A function called after the journal menu's initialize script runs.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterJournalMenuOpen(Internal::AfterJournalMenuOpenCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_journal_menu_open_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterJournalMenuOpenHook(callback);
		}

		/// Registers a callback that runs after the journal menu closes.
		/// @param callback A function called after the journal menu's close script runs.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterJournalMenuClose(Internal::AfterJournalMenuCloseCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_journal_menu_close_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterJournalMenuCloseHook(callback);
		}

		/// Registers a callback that runs after the store menu opens.
		/// @param callback A function called after the store menu's initialize script runs.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterStoreMenuOpen(Internal::AfterStoreMenuOpenCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_store_menu_open_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterStoreMenuOpenHook(callback);
		}

		/// Registers a callback that runs after the store menu closes.
		/// @param callback A function called after the store menu's close script runs.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterStoreMenuClose(Internal::AfterStoreMenuCloseCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_store_menu_close_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterStoreMenuCloseHook(callback);
		}

		/// Registers a callback that runs before the title menu's setup_main_screen script.
		/// MMAPI automatically clears its captured script and instance contexts before the callback fires,
		/// so mods returning to the title screen do not need to call ClearScriptContexts manually.
		/// @param callback A function called before the title menu setup script runs (after MMAPI clears its contexts).
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeSetupMainScreen(Internal::BeforeSetupMainScreenCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_setup_main_screen_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterBeforeSetupMainScreenHook(callback);
		}
	}
}
