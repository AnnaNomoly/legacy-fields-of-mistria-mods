#pragma once

#include "Core.hpp"
#include "Engine.hpp"
#include "Hook.hpp"
#include "Log.hpp"
#include "Status.hpp"

#include <string>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Display
{
	struct DisplayResizeContext
	{
		double m_window_width  = 0.0;
		double m_window_height = 0.0;

		/// Returns the window width in pixels at the moment the resize completed.
		double GetWindowWidth() const { return m_window_width; }

		/// Returns the window height in pixels at the moment the resize completed.
		double GetWindowHeight() const { return m_window_height; }
	};

	struct VertigoDrawWithColorContext
	{
		std::string  m_sprite_name;
		YYTK::RValue m_sprite_asset;

		/// Returns the sprite name being drawn, extracted from `Arguments[0]` via `sprite_get_name`.
		/// Empty if `Arguments[0]` is not a sprite asset (e.g. it's a font, surface, or non-asset value).
		std::string_view GetSpriteName() const { return m_sprite_name; }

		/// Overrides the sprite asset the game will draw in screen space.
		/// Typically populated with `MMAPI::Engine::AssetGetIndex("spr_...")`.
		void SetSpriteAsset(YYTK::RValue sprite_asset) { m_sprite_asset = sprite_asset; }
	};

	struct PlayHealVfxContext
	{
		bool m_cancelled = false;

		/// Prevents the game's play_heal_vfx script from running this fire.
		void Cancel() { m_cancelled = true; }
	};

	namespace Internal
	{
		inline bool enabled = false;

		inline constexpr const char* GML_SCRIPT_DISPLAY_RESIZE           = "gml_Script_resize_amount@Display@Display";
		inline constexpr const char* GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR  = "gml_Script_vertigo_draw_with_color";
		inline constexpr const char* GML_SCRIPT_PLAY_HEAL_VFX            = "gml_Script_play_heal_vfx";

		using AfterDisplayResizeCallback         = void(*)(MMAPI::Display::DisplayResizeContext&);
		using BeforeVertigoDrawWithColorCallback = void(*)(MMAPI::Display::VertigoDrawWithColorContext&);
		using BeforePlayHealVfxCallback          = void(*)(MMAPI::Display::PlayHealVfxContext&);

		inline AfterDisplayResizeCallback         after_display_resize_callback           = nullptr;
		inline BeforeVertigoDrawWithColorCallback before_vertigo_draw_with_color_callback = nullptr;
		inline BeforePlayHealVfxCallback          before_play_heal_vfx_callback           = nullptr;

		inline YYTK::RValue& GmlScriptAfterDisplayResizeCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_DISPLAY_RESIZE)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			if (after_display_resize_callback)
			{
				MMAPI::Display::DisplayResizeContext context{
					MMAPI::Engine::GetWindowWidth(),
					MMAPI::Engine::GetWindowHeight()
				};
				after_display_resize_callback(context);
			}

			return Result;
		}

		inline YYTK::RValue& GmlScriptBeforeVertigoDrawWithColorCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (before_vertigo_draw_with_color_callback && Arguments && ArgumentCount >= 1 && Arguments[0])
			{
				// arg0 may be any asset (sprite/font/etc.). Resolve a sprite name only when it's a sprite.
				std::string sprite_name;
				YYTK::RValue asset_type = MMAPI::Internal::module_interface->CallBuiltin(
					"asset_get_type", { *Arguments[0] }
				);
				constexpr int64_t asset_sprite = 1;
				if (asset_type.ToInt64() == asset_sprite)
				{
					YYTK::RValue name = MMAPI::Internal::module_interface->CallBuiltin(
						"sprite_get_name", { *Arguments[0] }
					);
					if (name.m_Kind == YYTK::VALUE_STRING)
						sprite_name = name.ToString();
				}

				MMAPI::Display::VertigoDrawWithColorContext context{ std::move(sprite_name), *Arguments[0] };
				before_vertigo_draw_with_color_callback(context);
				*Arguments[0] = context.m_sprite_asset;
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			return Result;
		}

		inline YYTK::RValue& GmlScriptBeforePlayHealVfxCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (before_play_heal_vfx_callback)
			{
				MMAPI::Display::PlayHealVfxContext context;
				before_play_heal_vfx_callback(context);

				if (context.m_cancelled)
					return Result;
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_PLAY_HEAL_VFX)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			return Result;
		}
	}

	/// Activates Display hooks. Installs the `resize_amount@Display@Display`,
	/// `vertigo_draw_with_color`, and `play_heal_vfx` script hooks so registered callbacks fire on
	/// display resize, screen-space sprite draws, and heal visual effects respectively. Safe to call
	/// before any Hooks::* registration — each callback no-ops until a user callback is bound.
	/// @return Status::Success if the hooks are installed (or already were); otherwise a failure status.
	inline MMAPI::Status Enable()
	{
		if (Internal::enabled)
			return MMAPI::Status::Success;

		MMAPI::Log::Debug("MMAPI::Display::Enable() called");

		MMAPI::Status status = MMAPI::Internal::InstallScriptHooks({
			{ Internal::GML_SCRIPT_DISPLAY_RESIZE,          reinterpret_cast<PVOID>(Internal::GmlScriptAfterDisplayResizeCallback) },
			{ Internal::GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR, reinterpret_cast<PVOID>(Internal::GmlScriptBeforeVertigoDrawWithColorCallback) },
			{ Internal::GML_SCRIPT_PLAY_HEAL_VFX,           reinterpret_cast<PVOID>(Internal::GmlScriptBeforePlayHealVfxCallback) },
		});
		if (!MMAPI::IsSuccess(status))
			return status;

		Internal::enabled = true;
		return MMAPI::Status::Success;
	}

	namespace Hooks
	{
		/// Registers a callback that runs after the game's `resize_amount@Display@Display` script.
		/// Use `ctx.GetWindowWidth()` / `ctx.GetWindowHeight()` to read the post-resize dimensions.
		/// @param callback A function called with a `MMAPI::Display::DisplayResizeContext`.
		/// @return Status::Success if the hook was installed; Status::AlreadyRegistered if a callback is already registered; otherwise a failure status.
		inline MMAPI::Status AfterDisplayResize(Internal::AfterDisplayResizeCallback callback)
		{
			MMAPI::Status status = MMAPI::Display::Enable();
			if (!MMAPI::IsSuccess(status))
				return status;

			return MMAPI::Internal::RegisterHook(
				"Display::AfterDisplayResize",
				Internal::after_display_resize_callback,
				callback
			);
		}

		/// Registers a callback that runs before the game's `vertigo_draw_with_color` script —
		/// the screen-space sprite draw routine used for HUD elements and other UI overlays.
		/// Use `ctx.GetSpriteName()` to identify the sprite being drawn and `ctx.SetSpriteAsset(...)`
		/// to override it (typically `MMAPI::Engine::AssetGetIndex("spr_...")`).
		/// For world-space sprite overrides (items dropped in the game world, etc.), use
		/// [`Item::Hooks::AfterGetUiIcon`](API-MMAPI-Item-Hooks-AfterGetUiIcon.md) instead.
		/// @param callback A function called with a mutable `MMAPI::Display::VertigoDrawWithColorContext`.
		/// @return Status::Success if the hook was installed; Status::AlreadyRegistered if a callback is already registered; otherwise a failure status.
		inline MMAPI::Status BeforeVertigoDrawWithColor(Internal::BeforeVertigoDrawWithColorCallback callback)
		{
			MMAPI::Status status = MMAPI::Display::Enable();
			if (!MMAPI::IsSuccess(status))
				return status;

			return MMAPI::Internal::RegisterHook(
				"Display::BeforeVertigoDrawWithColor",
				Internal::before_vertigo_draw_with_color_callback,
				callback
			);
		}

		/// Registers a callback that runs before the game's `play_heal_vfx` script. Call `ctx.Cancel()`
		/// to short-circuit the heal visual effect this fire (e.g. when a mod-controlled time-stop is
		/// active and the vfx shouldn't play).
		/// @param callback A function called with a mutable `MMAPI::Display::PlayHealVfxContext`.
		/// @return Status::Success if the hook was installed; Status::AlreadyRegistered if a callback is already registered; otherwise a failure status.
		inline MMAPI::Status BeforePlayHealVfx(Internal::BeforePlayHealVfxCallback callback)
		{
			MMAPI::Status status = MMAPI::Display::Enable();
			if (!MMAPI::IsSuccess(status))
				return status;

			return MMAPI::Internal::RegisterHook(
				"Display::BeforePlayHealVfx",
				Internal::before_play_heal_vfx_callback,
				callback
			);
		}
	}
}
