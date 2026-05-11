#pragma once

#include "Core.hpp"
#include "Instance.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Bark
{
	/// Source: globalInstance.__bark_id__
	enum class Icons : int
	{
		Adeline            = 0,
		Angry              = 1,
		Annoyed            = 2,
		Archaeology        = 3,
		Balor              = 4,
		Bathhouse          = 5,
		Beach              = 6,
		Blush              = 7,
		Book               = 8,
		BreathOfFlame      = 9,
		BreedFemale        = 10,
		BreedMale          = 11,
		Caldarus           = 12,
		Celebration        = 13,
		Celine             = 14,
		CherryBlossoms     = 15,
		Chicken            = 16,
		Coin               = 17,
		ColdDrink          = 18,
		Cooking            = 19,
		Cow                = 20,
		Crafting           = 21,
		Crown              = 22,
		CuteFace           = 23,
		Darcy              = 24,
		Dell               = 25,
		Dozy               = 26,
		Eight              = 27,
		Eiland             = 28,
		Ellipses           = 29,
		Elsie              = 30,
		EmptyHeart         = 31,
		Errol              = 32,
		ExclamationMark    = 33,
		Fall               = 34,
		Farming            = 35,
		Fire               = 36,
		Firesail           = 37,
		FishBait           = 38,
		Fishing            = 39,
		Five               = 40,
		Forest             = 41,
		Four               = 42,
		Gem                = 43,
		Hay                = 44,
		Hayden             = 45,
		Heart              = 46,
		Heatwave           = 47,
		Hemlock            = 48,
		Henrietta          = 49,
		Holt               = 50,
		Horse              = 51,
		HotDrink           = 52,
		Hungry             = 53,
		Josephine          = 54,
		Juniper            = 55,
		Lake               = 56,
		Landen             = 57,
		Louis              = 58,
		Luc                = 59,
		Maple              = 60,
		March              = 61,
		Merri              = 62,
		Mining             = 63,
		Mist               = 64,
		Moon               = 65,
		Mountain           = 66,
		Music              = 67,
		Nine               = 68,
		NoCoin             = 69,
		Nora               = 70,
		Obsidian           = 71,
		Olric              = 72,
		One                = 73,
		PlantTonic         = 74,
		Priestess          = 75,
		QuestionMark       = 76,
		Rainy              = 77,
		Reina              = 78,
		RelationshipStatus = 79,
		Ryis               = 80,
		Seed               = 81,
		Seven              = 82,
		Six                = 83,
		Sleepy             = 84,
		SmokeMoth          = 85,
		Snow               = 86,
		Spring             = 87,
		Stars              = 88,
		Summer             = 89,
		Sunny              = 90,
		SweatDrop          = 91,
		Terithia           = 92,
		Thread             = 93,
		Three              = 94,
		Thunderstorm       = 95,
		Two                = 96,
		Valen              = 97,
		Vera               = 98,
		Wheedle            = 99,
		Winter             = 100,
		Woodcrafting       = 101,
		Yum                = 102
	};

	/// Total number of enumerators in Icons. Iterating [0, IconCount) covers every Icons value.
	inline constexpr int IconCount = 103;

	/// Invokes fn with every Icons value, in ascending order.
	template <typename Fn>
	inline void ForEachIcon(Fn fn)
	{
		for (int i = 0; i < IconCount; ++i)
			fn(static_cast<Icons>(i));
	}

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_BARK_EMITTER      = "gml_Script_BarkEmitter";
		inline constexpr const char* GML_SCRIPT_BARK_EMITTER_EMIT = "gml_Script_emit@BarkEmitter@BarkEmitter";

		// Live BarkEmitter Self, latched on the first BarkEmitter call (the free constructor-style script).
		// BarkEmitter is a singleton created early in the game session; the latched Self stays valid until return-to-title.
		inline YYTK::CInstance* bark_emitter_self = nullptr;

		// Cleared from the setup_main_screen pub/sub when the player returns to the title menu.
		// Registered by Bark::Enable().
		inline void ClearBarkEmitterOnReturnToTitle(YYTK::CInstance* /*Self*/, YYTK::CInstance* /*Other*/)
		{
			bark_emitter_self = nullptr;
		}

		inline YYTK::RValue& BarkEmitterContextCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			// Refresh on every fire so we always have the freshest BarkEmitter Self.
			bark_emitter_self = Self;

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_BARK_EMITTER)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}

		/// Resolves the calling context for `emit@BarkEmitter@BarkEmitter` — Self from the latched BarkEmitter,
		/// Other from the obj_ari instance (so the bark appears over Ari).
		/// @return True if both pointers were resolved, false otherwise.
		inline bool TryGetBarkEmitterContext(YYTK::CInstance*& Self, YYTK::CInstance*& Other)
		{
			if (!bark_emitter_self)
				return false;

			YYTK::CInstance* ari_struct   = nullptr;
			YYTK::CInstance* ari_instance = nullptr;
			if (!MMAPI::Instance::Internal::TryGetAriContext(ari_struct, ari_instance))
				return false;

			Self  = bark_emitter_self;
			Other = ari_instance;
			return true;
		}
	}

	/// Activates Bark utility functions. Installs the BarkEmitter hook so the live BarkEmitter Self is latched
	/// for TryGetBarkEmitterContext (cleared on return-to-title via the setup_main_screen pub/sub).
	/// Cascades to MMAPI::Instance::Enable so the obj_ari instance is available.
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		Aurie::AurieStatus status = MMAPI::Instance::Enable();
		if (!Aurie::AurieSuccess(status))
			return status;

		MMAPI::Internal::RegisterOnSetupMainScreenHandler(Internal::ClearBarkEmitterOnReturnToTitle);

		return MMAPI::Internal::InstallScriptHooks({
			{ MMAPI::Internal::GML_SCRIPT_SETUP_MAIN_SCREEN, reinterpret_cast<PVOID>(MMAPI::Internal::GmlScriptBeforeSetupMainScreenCallback) },
			{ Internal::GML_SCRIPT_BARK_EMITTER,             reinterpret_cast<PVOID>(Internal::BarkEmitterContextCallback) },
		});
	}

	/// Emits a bark (voiced dialogue bubble) over Ari with the given icon.
	/// @attention Requires MMAPI::Bark::Enable() to have been called.
	/// @param icon The bark icon to display.
	/// @param bark_type An engine-specific bark type code; defaults to 0.
	/// @return True if the bark was emitted, false if the required context is unavailable.
	inline bool Emit(MMAPI::Bark::Icons icon, int bark_type = 0)
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!Internal::TryGetBarkEmitterContext(Self, Other))
			return false;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_BARK_EMITTER_EMIT, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue bark_id    = static_cast<int>(icon);
		YYTK::RValue bark_type_value = bark_type;
		YYTK::RValue result;
		YYTK::RValue* args[2] = { &bark_id, &bark_type_value };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 2, args);
		return true;
	}
}
