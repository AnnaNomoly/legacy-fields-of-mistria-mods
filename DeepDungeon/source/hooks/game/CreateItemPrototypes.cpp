#include "../../utils/Utils.h"

using namespace State::Maps;

// MMAPI Item::Hooks::AfterCreateItemPrototypes callback.
void AfterCreateItemPrototypes(MMAPI::Item::CreateItemPrototypesContext& ctx)
{
	const size_t count = ctx.Count();
	YYTK::RValue prototypes = ctx.GetItemPrototypes();

	for (size_t i = 0; i < count; i++)
	{
		YYTK::RValue* entry = nullptr;
		g_ModuleInterface->GetArrayEntry(prototypes, i, entry);
		if (entry)
			item_id_to_prototype_map[i] = *entry;
	}
}
