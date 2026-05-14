#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::UI;

// MMAPI Display::Hooks::BeforeVertigoDrawWithColor callback.
void BeforeVertigoDrawWithColor(MMAPI::Display::VertigoDrawWithColorContext& ctx)
{
	if (!game_is_active || crafting_menu_open || !AriCurrentGmRoomIsDungeonFloor())
		return;

	const std::string sprite_name{ ctx.GetSpriteName() };
	if (sprite_name.empty())
		return;

	RValue dynamic_sprite = GetDynamicUiSprite(sprite_name);
	if (dynamic_sprite.m_Kind == VALUE_REF)
		ctx.SetSpriteAsset(dynamic_sprite);
}
