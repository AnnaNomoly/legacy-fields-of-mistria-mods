#include "../../utils/Utils.h"

RValue& GmlScriptAriFaceDirCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	ari_facing_dir = Arguments[0]->ToDouble();

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ARI_FACE_DIR));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
