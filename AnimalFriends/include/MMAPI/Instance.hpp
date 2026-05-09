#pragma once

#include <string_view>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Instance
{
	namespace Internal
	{
		inline constexpr const char* INSTANCE_OBJ_ARI        = "obj_ari";
		inline constexpr const char* INSTANCE_OBJ_ADELINE    = "obj_adeline";
		inline constexpr const char* INSTANCE_OBJ_BALOR      = "obj_balor";
		inline constexpr const char* INSTANCE_OBJ_CALDARUS   = "obj_caldarus";
		inline constexpr const char* INSTANCE_OBJ_CELINE     = "obj_celine";
		inline constexpr const char* INSTANCE_OBJ_DARCY      = "obj_darcy";
		inline constexpr const char* INSTANCE_OBJ_DELL       = "obj_dell";
		inline constexpr const char* INSTANCE_OBJ_DOZY       = "obj_dozy";
		inline constexpr const char* INSTANCE_OBJ_EILAND     = "obj_eiland";
		inline constexpr const char* INSTANCE_OBJ_ELSIE      = "obj_elsie";
		inline constexpr const char* INSTANCE_OBJ_ERROL      = "obj_errol";
		inline constexpr const char* INSTANCE_OBJ_HAYDEN     = "obj_hayden";
		inline constexpr const char* INSTANCE_OBJ_HEMLOCK    = "obj_hemlock";
		inline constexpr const char* INSTANCE_OBJ_HENRIETTA  = "obj_henrietta";
		inline constexpr const char* INSTANCE_OBJ_HOLT       = "obj_holt";
		inline constexpr const char* INSTANCE_OBJ_JOSEPHINE  = "obj_josephine";
		inline constexpr const char* INSTANCE_OBJ_JUNIPER    = "obj_juniper";
		inline constexpr const char* INSTANCE_OBJ_LANDEN     = "obj_landen";
		inline constexpr const char* INSTANCE_OBJ_LOUIS      = "obj_louis";
		inline constexpr const char* INSTANCE_OBJ_LUC        = "obj_luc";
		inline constexpr const char* INSTANCE_OBJ_MAPLE      = "obj_maple";
		inline constexpr const char* INSTANCE_OBJ_MARCH      = "obj_march";
		inline constexpr const char* INSTANCE_OBJ_MERRI      = "obj_merri";
		inline constexpr const char* INSTANCE_OBJ_NORA       = "obj_nora";
		inline constexpr const char* INSTANCE_OBJ_OLRIC      = "obj_olric";
		inline constexpr const char* INSTANCE_OBJ_REINA      = "obj_reina";
		inline constexpr const char* INSTANCE_OBJ_RYIS       = "obj_ryis";
		inline constexpr const char* INSTANCE_OBJ_SERIDIA    = "obj_seridia";
		inline constexpr const char* INSTANCE_OBJ_STILLWELL  = "obj_stillwell";
		inline constexpr const char* INSTANCE_OBJ_TALIFERRO  = "obj_taliferro";
		inline constexpr const char* INSTANCE_OBJ_TERITHIA   = "obj_terithia";
		inline constexpr const char* INSTANCE_OBJ_VALEN      = "obj_valen";
		inline constexpr const char* INSTANCE_OBJ_VERA       = "obj_vera";
		inline constexpr const char* INSTANCE_OBJ_WHEEDLE    = "obj_wheedle";
		inline constexpr const char* INSTANCE_OBJ_ZOREL      = "obj_zorel";
	}

	/// Returns true if the instance's GML object name exactly matches object_name.
	/// @param instance The YYTK instance to check.
	/// @param object_name The exact GML object name to match.
	inline bool IsNamed(YYTK::CInstance* instance, std::string_view object_name)
	{
		return instance &&
		       instance->m_Object &&
		       instance->m_Object->m_Name &&
		       std::string_view(instance->m_Object->m_Name) == object_name;
	}

	/// Returns true if the instance's GML object name contains object_name_part.
	/// @param instance The YYTK instance to check.
	/// @param object_name_part The GML object name fragment to search for.
	inline bool NameContains(YYTK::CInstance* instance, std::string_view object_name_part)
	{
		return instance &&
		       instance->m_Object &&
		       instance->m_Object->m_Name &&
		       std::string_view(instance->m_Object->m_Name).find(object_name_part) != std::string_view::npos;
	}
}
