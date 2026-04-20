#include "../../utils/Utils.h"

using namespace State::Player;

static void ParseSavePrefix(RValue** Arguments)
{
	std::string save_file = Arguments[0]->ToInstance()->GetMember("save_path").ToString();
	std::size_t save_file_name_delimiter_index = save_file.find_last_of("/");
	std::string save_name = save_file.substr(save_file_name_delimiter_index + 1);

	std::size_t first_hyphen_index = save_name.find_first_of("-") + 1;
	std::size_t second_hyphen_index = save_name.find_last_of("-");
	save_prefix = save_name.substr(first_hyphen_index, (second_hyphen_index - first_hyphen_index));
}

RValue& GmlScriptLoadGameCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	ParseSavePrefix(Arguments);

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_LOAD_GAME));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
