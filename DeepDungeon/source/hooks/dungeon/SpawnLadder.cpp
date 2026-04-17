#include "../../utils/Utils.h"

RValue& GmlScriptSpawnLadderCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (ari_current_gm_room.contains("seal") || ari_current_gm_room.contains("ritual_chamber"))
		return Result;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SPAWN_LADDER));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
